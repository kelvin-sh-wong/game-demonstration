#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "collider.hpp"
#include "fixed_rotation.hpp"
#include "motion.hpp"
#include "movement.hpp"
#include "rigid_body.hpp"
#include "transformation.hpp"

#include <memory>

class Entity {
public:
    Entity();

    std::unique_ptr<Transformation> transformation;
    std::unique_ptr<Motion> motion;
    std::unique_ptr<Collider> collider;
    std::unique_ptr<RigidBody> rigid_body;
    std::unique_ptr<Movement> movement;
    std::unique_ptr<FixedRotation> fixed_rotation;
};

#include "collider.hpp"
#include "serialize.hpp"
#include "transformation.hpp"

#include <bitset>
#include <cstdint>
#include <ostream>

template <>
struct serialize<Entity> {
    void operator()(std::ostream & ostream, Entity const & entity) const {
        std::bitset<2> component_indices;

        if (entity.transformation != nullptr) {
            component_indices.set(0);
        }
        if (entity.collider != nullptr) {
            component_indices.set(1);
        }

        serialize<std::uint8_t>{}(ostream, static_cast<std::uint8_t>(component_indices.to_ulong()));

        if (entity.transformation != nullptr) {
            serialize<Transformation>{}(ostream, *entity.transformation);
        }
        if (entity.collider != nullptr) {
            serialize<Collider>{}(ostream, *entity.collider);
        }
    }
};

#endif  // ENTITY_HPP
