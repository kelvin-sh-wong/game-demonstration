#ifndef SNAPSHOT_HPP
#define SNAPSHOT_HPP

#include "entity.hpp"

#include <vector>

class Snapshot {
public:
    Snapshot(std::vector<Entity> entities, double time);

    std::vector<Entity> const & entities() const;
    double time() const;

private:
    std::vector<Entity> entities_;
    double time_;
};

#include "deserialize.hpp"
#include "entity.hpp"

#include <istream>
#include <utility>
#include <vector>

template <>
struct deserialize<Snapshot> {
    Snapshot operator()(std::istream & istream) const {
        std::vector<Entity> entities{deserialize<std::vector<Entity>>{}(istream)};
        double time{deserialize<double>{}(istream)};

        return Snapshot{std::move(entities), time};
    }
};

#endif  // SNAPSHOT_HPP
