#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "entity.hpp"

#include <vector>

class Registry {
public:
    Registry();

    std::vector<Entity> entities;
};

#endif  // REGISTRY_HPP
