// Game Client

#define GLFW_INCLUDE_NONE

#include "action.hpp"
#include "action_type.hpp"
#include "address.hpp"
#include "collider.hpp"
#include "convex_polygon.hpp"
#include "deserialize.hpp"
#include "entity.hpp"
#include "file_loading_error.hpp"
#include "glad_error.hpp"
#include "glfw_error.hpp"
#include "message_type.hpp"
#include "player.hpp"
#include "program.hpp"
#include "program_linking_error.hpp"
#include "registry.hpp"
#include "resource_manager.hpp"
#include "serialize.hpp"
#include "shader.hpp"
#include "shader_compilation_error.hpp"
#include "shape_renderer.hpp"
#include "snapshot.hpp"
#include "socket.hpp"
#include "transformation.hpp"
#include "vector2.hpp"
#include "window.hpp"
#include "wsa_error.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <winsock2.h>

#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace {

constexpr char const * server_ip_address{"127.0.0.1"};
constexpr char const * server_port_number{"32768"};
constexpr double min_login_attempt_interval{1.0};

std::unique_ptr<Socket> socket;
std::unique_ptr<Registry> registry;
std::unique_ptr<Player> player;
std::unique_ptr<ResourceManager> resource_manager;
std::unique_ptr<Window> window;
std::unique_ptr<ShapeRenderer> shape_renderer;

void send_login(double client_time) {
    if (player->entity_id() != 0) {
        return;
    }

    if (player->login_attempted_time() >= 0.0 && client_time - player->login_attempted_time() < min_login_attempt_interval) {
        return;
    }

    std::ostringstream ostringstream;
    serialize<MessageType>{}(ostringstream, MessageType::login);

    socket->send(Address{server_ip_address, server_port_number}, ostringstream.str());

    player->set_login_attempted_time(client_time);
}

void send_movement_action() {
    if (player->entity_id() == 0) {
        return;
    }

    Vector2 direction{0.0, 0.0};

    if (window->keys().find(GLFW_KEY_W) != window->keys().end()) {
        direction.y += 1.0;
    }
    if (window->keys().find(GLFW_KEY_A) != window->keys().end()) {
        direction.x -= 1.0;
    }
    if (window->keys().find(GLFW_KEY_S) != window->keys().end()) {
        direction.y -= 1.0;
    }
    if (window->keys().find(GLFW_KEY_D) != window->keys().end()) {
        direction.x += 1.0;
    }

    Action action{ActionType::movement, std::vector<Vector2>{std::move(direction)}};

    std::ostringstream ostringstream;
    serialize<MessageType>{}(ostringstream, MessageType::action);
    serialize<Action>{}(ostringstream, action);

    socket->send(Address{server_ip_address, server_port_number}, ostringstream.str());
}

void receive_login_response(std::string message) {
    if (player->entity_id() != 0) {
        return;
    }

    std::istringstream istringstream{std::move(message)};
    MessageType message_type{deserialize<MessageType>{}(istringstream)};
    std::uint32_t entity_id{deserialize<std::uint32_t>{}(istringstream)};

    player->set_entity_id(entity_id);

    std::cout << std::format("Logged in to Server [/{}:{}].\n", server_ip_address, server_port_number);
}

void receive_snapshot(std::string message, double client_time) {
    if (player->entity_id() == 0) {
        return;
    }

    std::istringstream istringstream{std::move(message)};
    MessageType message_type{deserialize<MessageType>{}(istringstream)};
    Snapshot snapshot{deserialize<Snapshot>{}(istringstream)};

    player->add_snapshot(std::move(snapshot), client_time);
}

void send(double client_time) {
    send_login(client_time);

    send_movement_action();
}

void receive(double client_time) {
    for (auto & [address, message] : socket->receive()) {
        if (address != Address{server_ip_address, server_port_number}) {
            continue;
        }

        std::istringstream istringstream{message.substr(0, sizeof(MessageType))};
        MessageType message_type{deserialize<MessageType>{}(istringstream)};

        switch (message_type) {
        case MessageType::login_response:
            receive_login_response(std::move(message));
            break;
        case MessageType::snapshot:
            receive_snapshot(std::move(message), client_time);
            break;
        }
    }
}

void update(double client_time) {
    player->interpolate_entities(*registry, client_time);
}

void render() {
    glViewport(0, 0, window->width(), window->height());
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 view{glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 1.0f, 0.0f})};
    glm::mat4 projection{glm::ortho(-window->width() * 0.5f, window->width() * 0.5f, -window->height() * 0.5f, window->height() * 0.5f, -1.0f, 1.0f)};

    for (std::size_t entity_id{0}; entity_id < registry->entities.size(); ++entity_id) {
        Entity const & entity{registry->entities[entity_id]};

        if (entity.transformation == nullptr || entity.collider == nullptr) {
            continue;
        }

        Transformation const & transformation{*entity.transformation};
        Collider const & collider{*entity.collider};

        glm::vec4 color{0.8f, 0.8f, 0.8f, 1.0f};

        if (entity_id == player->entity_id()) {
            color = glm::vec4{0.0f, 1.0f, 0.0f, 1.0f};
        }

        if (std::holds_alternative<ConvexPolygon>(collider.shape())) {
            ConvexPolygon convex_polygon{std::get<ConvexPolygon>(collider.shape())};
            convex_polygon.rotate(transformation.rotation());
            convex_polygon.translate(transformation.position());

            shape_renderer->draw_shape(convex_polygon, view, projection, color);
        }
        else {
            __assume(0);
        }
    }
}

void run() {
    std::cout << "Client started.\n";

    try {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            throw WSAError{"Failed to initialize WSA.", WSAGetLastError()};
        }

        if (!glfwInit()) {
            throw GLFWError{"Failed to initialize GLFW.", glfwGetError(nullptr)};
        }

        socket = std::make_unique<Socket>(Address{"0.0.0.0", "0"});

        registry = std::make_unique<Registry>();

        player = std::make_unique<Player>();

        resource_manager = std::make_unique<ResourceManager>();

        resource_manager->load_file("primitive.vert", "resources/primitive.vert");
        resource_manager->load_file("primitive.frag", "resources/primitive.frag");

        window = std::make_unique<Window>(800, 600, "Game");

        {
            Shader vertex_shader{GL_VERTEX_SHADER, resource_manager->get_file("primitive.vert")};
            Shader fragment_shader{GL_FRAGMENT_SHADER, resource_manager->get_file("primitive.frag")};
            Program program{std::vector<Shader const *>{&vertex_shader, &fragment_shader}};

            shape_renderer = std::make_unique<ShapeRenderer>(std::move(program));
        }

        glEnable(GL_BLEND);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        double previous_time{glfwGetTime()};

        while (!window->should_close()) {
            double current_time{glfwGetTime()};

            receive(current_time);

            send(current_time);

            update(current_time);

            render();

            window->swap_buffers();

            glfwPollEvents();

            previous_time = current_time;
        }
    }
    catch (FileLoadingError const & error) {
        std::cout << error.what() << "\n";
    }
    catch (GLADError const & error) {
        std::cout << error.what() << "\n";
    }
    catch (GLFWError const & error) {
        std::cout << error.what() << "\n";
    }
    catch (ProgramLinkingError const & error) {
        std::cout << error.what() << "\n";
    }
    catch (ShaderCompilationError const & error) {
        std::cout << error.what() << "\n";
    }
    catch (WSAError const & error) {
        std::cout << error.what() << "\n";
    }

    shape_renderer.reset();
    window.reset();
    resource_manager.reset();
    player.reset();
    registry.reset();
    socket.reset();

    glfwTerminate();
    WSACleanup();
}

}

int main() {
    run();

    return 0;
}
