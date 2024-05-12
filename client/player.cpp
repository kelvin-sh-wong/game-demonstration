#include "player.hpp"

#include "collider.hpp"
#include "entity.hpp"
#include "registry.hpp"
#include "snapshot.hpp"
#include "transformation.hpp"
#include "vector2.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace {

Entity interpolate(Entity const & entity_1, Entity const & entity_2, double weight) {
    Entity entity;

    if (entity_1.transformation != nullptr && entity_2.transformation != nullptr) {
        Transformation const & transformation_1{*entity_1.transformation};
        Transformation const & transformation_2{*entity_2.transformation};

        Vector2 const & position_1{transformation_1.position()};
        Vector2 const & position_2{transformation_2.position()};

        double rotation_1{transformation_1.rotation()};
        double rotation_2{transformation_2.rotation()};

        Vector2 position{std::lerp(position_1.x, position_2.x, weight), std::lerp(position_1.y, position_2.y, weight)};
        double rotation{std::lerp(rotation_1, rotation_2, weight)};

        Transformation transformation{std::move(position), rotation};

        entity.transformation = std::make_unique<Transformation>(std::move(transformation));
    }
    else if (entity_2.transformation != nullptr && weight >= 0.0) {
        entity.transformation = std::make_unique<Transformation>(*entity_2.transformation);
    }
    else if (entity_1.transformation != nullptr && weight < 0.0) {
        entity.transformation = std::make_unique<Transformation>(*entity_1.transformation);
    }

    if (entity_2.collider != nullptr && weight >= 0.0) {
        entity.collider = std::make_unique<Collider>(*entity_2.collider);
    }
    else if (entity_1.collider != nullptr && weight < 0.0) {
        entity.collider = std::make_unique<Collider>(*entity_1.collider);
    }

    return entity;
}

}

Player::Player() : entity_id_{0}, login_attempted_time_{-1.0}, interpolation_offset_{0.0}, smoothed_snapshot_latency_{0.0}, snapshot_updated_time_{-1.0} {
    // empty
}

std::uint32_t Player::entity_id() const {
    return entity_id_;
}

void Player::set_entity_id(std::uint32_t entity_id) {
    entity_id_ = entity_id;
}

double Player::login_attempted_time() const {
    return login_attempted_time_;
}

void Player::set_login_attempted_time(double login_attempted_time) {
    assert(login_attempted_time >= 0.0);

    login_attempted_time_ = login_attempted_time;
}

void Player::interpolate_entities(Registry & registry, double client_time) const {
    assert(client_time >= 0.0);

    if (snapshots_.size() < 2) {
        registry.entities = std::vector<Entity>{};
        return;
    }

    double interpolation_time{client_time - interpolation_offset_};

    if (interpolation_time < snapshots_.front().time()) {
        registry.entities = std::vector<Entity>{};
        return;
    }

    std::size_t snapshot_index;
    for (snapshot_index = 0; snapshot_index < snapshots_.size() - 2; ++snapshot_index) {
        if (interpolation_time < snapshots_[snapshot_index + 1].time()) {
            break;
        }
    }

    Snapshot const & snapshot_1{snapshots_[snapshot_index]};
    Snapshot const & snapshot_2{snapshots_[snapshot_index + 1]};

    std::vector<Entity> const & entities_1{snapshot_1.entities()};
    std::vector<Entity> const & entities_2{snapshot_2.entities()};
    double weight{(interpolation_time - snapshot_1.time()) / (snapshot_2.time() - snapshot_1.time())};

    std::vector<Entity> entities;

    for (std::size_t i{0}; i < entities_2.size(); ++i) {
        if (i < entities_1.size()) {
            Entity entity{interpolate(entities_1[i], entities_2[i], weight)};
            entities.push_back(std::move(entity));
        }
        else {
            Entity entity{interpolate(entities_2[i], entities_2[i], 0.0)};
            entities.push_back(std::move(entity));
        }
    }

    registry.entities = std::move(entities);
}

void Player::add_snapshot(Snapshot snapshot, double client_time) {
    assert(client_time >= 0.0);
    assert(client_time >= snapshot_updated_time_);

    if (snapshots_.size() > 0 && snapshot.time() <= snapshots_.back().time()) {
        return;
    }

    double snapshot_latency{client_time - snapshot.time()};

    if (snapshots_.size() == 0) {
        smoothed_snapshot_latency_ = snapshot_latency;
    }
    else {
        smoothed_snapshot_latency_ = smoothed_snapshot_latency_ * (1.0 - 0.125) + snapshot_latency * 0.125;
    }

    double expected_interpolation_offset{smoothed_snapshot_latency_ + snapshot_interval * 2.0};

    if (snapshots_.size() == 0 || std::abs(expected_interpolation_offset - interpolation_offset_) > snapshot_interval) {
        interpolation_offset_ = expected_interpolation_offset;
    }

    while (snapshots_.size() >= 2 && client_time - std::max(interpolation_offset_, snapshot_latency + snapshot_interval * 2.0) >= snapshots_[1].time()) {
        snapshots_.pop_front();
    }

    snapshots_.push_back(std::move(snapshot));

    snapshot_updated_time_ = client_time;
}
