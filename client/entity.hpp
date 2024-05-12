#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "collider.hpp"
#include "transformation.hpp"

#include <memory>

class Entity {
public:
    Entity();

    std::unique_ptr<Transformation> transformation;
    std::unique_ptr<Collider> collider;
};

#include "collider.hpp"
#include "deserialize.hpp"
#include "transformation.hpp"

#include <bitset>
#include <cstdint>
#include <istream>
#include <memory>
#include <utility>

template <>
struct deserialize<Entity> {
    Entity operator()(std::istream & istream) const {
        Entity entity;

        std::bitset<2> component_indices{deserialize<std::uint8_t>{}(istream)};

        if (component_indices.test(0)) {
            Transformation transformation{deserialize<Transformation>{}(istream)};
            entity.transformation = std::make_unique<Transformation>(std::move(transformation));
        }
        if (component_indices.test(1)) {
            Collider collider{deserialize<Collider>{}(istream)};
            entity.collider = std::make_unique<Collider>(std::move(collider));
        }

        return entity;
    }
};

#endif  // ENTITY_HPP
