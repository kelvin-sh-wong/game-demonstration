#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "entity.hpp"

#include <vector>

class Registry {
public:
    Registry();

    std::vector<Entity> entities;
};

#include "entity.hpp"
#include "serialize.hpp"

#include <ostream>
#include <vector>

template <>
struct serialize<Registry> {
    void operator()(std::ostream & ostream, Registry const & registry) const {
        serialize<std::vector<Entity>>{}(ostream, registry.entities);
    }
};

#endif  // REGISTRY_HPP
