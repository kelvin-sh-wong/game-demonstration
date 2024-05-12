#include "snapshot.hpp"

#include "entity.hpp"

#include <cassert>
#include <utility>
#include <vector>

Snapshot::Snapshot(std::vector<Entity> entities, double time) : entities_{std::move(entities)}, time_{time} {
    assert(time_ >= 0.0);
}

std::vector<Entity> const & Snapshot::entities() const {
    return entities_;
}

double Snapshot::time() const {
    return time_;
}
