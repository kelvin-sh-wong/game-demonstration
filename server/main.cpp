// Game Server

#include "action.hpp"
#include "action_type.hpp"
#include "address.hpp"
#include "collider.hpp"
#include "constraint_solver.hpp"
#include "contact_generator.hpp"
#include "convex_polygon.hpp"
#include "deserialize.hpp"
#include "entity.hpp"
#include "fixed_rotation.hpp"
#include "message_type.hpp"
#include "motion.hpp"
#include "movement.hpp"
#include "player.hpp"
#include "registry.hpp"
#include "rigid_body.hpp"
#include "serialize.hpp"
#include "socket.hpp"
#include "transformation.hpp"
#include "vector2.hpp"
#include "wsa_error.hpp"

#include <winsock2.h>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr double tick{1.0 / 64.0};

std::unique_ptr<Socket> socket;
std::unique_ptr<Registry> registry;
std::unique_ptr<std::unordered_map<Address, Player>> players;
std::unique_ptr<ContactGenerator> contact_generator;
std::unique_ptr<ConstraintSolver> constraint_solver;

void send_login_response(Address const & address) {
    if (players->find(address) == players->end()) {
        return;
    }

    Player const & player{players->at(address)};

    if (player.entity_id() == 0) {
        return;
    }

    std::ostringstream ostringstream;
    serialize<MessageType>{}(ostringstream, MessageType::login_response);
    serialize<std::uint32_t>{}(ostringstream, player.entity_id());

    socket->send(address, ostringstream.str());
}

void send_snapshot(Address const & address, double server_time) {
    if (players->find(address) == players->end()) {
        return;
    }

    Player const & player{players->at(address)};

    if (player.entity_id() == 0) {
        return;
    }

    std::ostringstream ostringstream;
    serialize<MessageType>{}(ostringstream, MessageType::snapshot);
    serialize<Registry>{}(ostringstream, *registry);
    serialize<double>{}(ostringstream, server_time);

    socket->send(address, ostringstream.str());
}

void receive_login(Address const & address, std::string message) {
    std::istringstream istringstream{std::move(message)};
    MessageType message_type{deserialize<MessageType>{}(istringstream)};

    if (players->find(address) == players->end()) {
        players->insert(std::make_pair(address, Player{address}));
    }

    Player & player{players->at(address)};

    if (player.entity_id() == 0) {
        Entity entity;
        entity.transformation = std::make_unique<Transformation>(Vector2{0.0, 0.0}, 0.0);
        entity.motion = std::make_unique<Motion>();
        entity.collider = std::make_unique<Collider>(ConvexPolygon{std::vector<Vector2>{Vector2{20.0, -20.0}, Vector2{20.0, 20.0}, Vector2{-20.0, 20.0}, Vector2{-20.0, -20.0}}});
        entity.rigid_body = std::make_unique<RigidBody>(1.0);
        entity.movement = std::make_unique<Movement>();
        entity.movement->set_speed(200.0);
        entity.fixed_rotation = std::make_unique<FixedRotation>(0.0);

        registry->entities.push_back(std::move(entity));

        player.set_entity_id(static_cast<std::uint32_t>(registry->entities.size() - 1));

        std::cout << std::format("Client [/{}:{}] logged in.\n", address.node(), address.service());
    }

    send_login_response(address);
}

void receive_action(Address const & address, std::string message) {
    if (players->find(address) == players->end()) {
        return;
    }

    Player & player{players->at(address)};

    if (player.entity_id() == 0) {
        return;
    }

    std::istringstream istringstream{std::move(message)};
    MessageType message_type{deserialize<MessageType>{}(istringstream)};
    Action action{deserialize<Action>{}(istringstream)};

    player.set_action(std::move(action));
}

void send(double server_time) {
    for (Address const & address : *players | std::views::keys) {
        send_snapshot(address, server_time);
    }
}

void receive() {
    for (auto & [address, message] : socket->receive()) {
        std::istringstream istringstream{message.substr(0, sizeof(MessageType))};
        MessageType message_type{deserialize<MessageType>{}(istringstream)};

        switch (message_type) {
        case MessageType::login:
            receive_login(address, std::move(message));
            break;
        case MessageType::action:
            receive_action(address, std::move(message));
            break;
        }
    }
}

void update(double delta_time) {
    for (Player & player : *players | std::views::values) {
        if (player.actions().find(ActionType::movement) == player.actions().end()) {
            continue;
        }

        Action const & action{player.actions().at(ActionType::movement)};

        if (action.values().size() < 1) {
            continue;
        }

        Vector2 const & direction{action.values()[0]};

        if (!std::isfinite(length(direction)) || length(direction) > 2.0) {
            continue;
        }

        if (player.entity_id() >= registry->entities.size()) {
            continue;
        }

        Entity & entity{registry->entities[player.entity_id()]};

        if (entity.movement == nullptr) {
            continue;
        }

        Movement & movement{*entity.movement};

        movement.set_direction(direction);
    }

    for (Entity & entity : registry->entities) {
        if (entity.motion == nullptr) {
            continue;
        }

        Motion & motion{*entity.motion};

        motion.set_linear_velocity(Vector2{0.0, 0.0});
        motion.set_angular_velocity(0.0);
    }

    for (Entity & entity : registry->entities) {
        if (entity.motion == nullptr || entity.movement == nullptr) {
            continue;
        }

        Motion & motion{*entity.motion};
        Movement const & movement{*entity.movement};

        motion.set_linear_velocity(movement.direction() * movement.speed());
    }

    contact_generator->generate_contacts(*registry);

    constraint_solver->solve_constraints(*registry, delta_time);

    for (Entity & entity : registry->entities) {
        if (entity.transformation == nullptr || entity.motion == nullptr) {
            continue;
        }

        Transformation & transformation{*entity.transformation};
        Motion const & motion{*entity.motion};

        transformation.set_position(transformation.position() + motion.linear_velocity() * delta_time);
        transformation.set_rotation(transformation.rotation() + motion.angular_velocity() * delta_time);
    }
}

void run() {
    std::cout << "Server started.\n";

    try {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            throw WSAError{"Failed to initialize WSA.", WSAGetLastError()};
        }

        std::chrono::high_resolution_clock::time_point start_time{std::chrono::high_resolution_clock::now()};

        socket = std::make_unique<Socket>(Address{"0.0.0.0", "32768"});

        registry = std::make_unique<Registry>();

        registry->entities.push_back(Entity{});

        {
            Entity entity;
            entity.transformation = std::make_unique<Transformation>(Vector2{0.0, -250.0}, 0.0);
            entity.motion = std::make_unique<Motion>();
            entity.collider = std::make_unique<Collider>(ConvexPolygon{std::vector<Vector2>{Vector2{375.0, -25.0}, Vector2{375.0, 25.0}, Vector2{-375.0, 25.0}, Vector2{-375.0, -25.0}}});
            entity.rigid_body = std::make_unique<RigidBody>(std::numeric_limits<double>::infinity());

            registry->entities.push_back(std::move(entity));
        }

        {
            Entity entity;
            entity.transformation = std::make_unique<Transformation>(Vector2{0.0, -140.0}, 0.0);
            entity.motion = std::make_unique<Motion>();
            entity.collider = std::make_unique<Collider>(ConvexPolygon{std::vector<Vector2>{Vector2{20.0, -80.0}, Vector2{20.0, 80.0}, Vector2{-20.0, 80.0}, Vector2{-20.0, -80.0}}});
            entity.rigid_body = std::make_unique<RigidBody>(8.0);

            registry->entities.push_back(std::move(entity));
        }

        players = std::make_unique<std::unordered_map<Address, Player>>();

        double previous_time{std::chrono::duration<double>{std::chrono::high_resolution_clock::now() - start_time}.count()};

        double accumulated_time{0.0};

        while (true) {
            double current_time{std::chrono::duration<double>{std::chrono::high_resolution_clock::now() - start_time}.count()};

            double delta_time{current_time - previous_time};

            accumulated_time += delta_time;

            while (accumulated_time >= tick) {
                receive();

                update(tick);

                send(current_time);

                accumulated_time -= tick;
            }

            previous_time = current_time;
        }
    }
    catch (WSAError const & error) {
        std::cout << error.what() << "\n";
    }

    players.reset();
    registry.reset();
    socket.reset();

    WSACleanup();
}

}

int main() {
    run();

    return 0;
}
