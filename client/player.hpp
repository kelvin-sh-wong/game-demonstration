#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "registry.hpp"
#include "snapshot.hpp"

#include <cstdint>
#include <deque>

class Player {
public:
    Player();

    std::uint32_t entity_id() const;
    void set_entity_id(std::uint32_t entity_id);

    double login_attempted_time() const;
    void set_login_attempted_time(double login_attempted_time);

    void interpolate_entities(Registry & registry, double client_time) const;
    void add_snapshot(Snapshot snapshot, double client_time);

private:
    static constexpr double snapshot_interval{1.0 / 64.0};

    std::uint32_t entity_id_;

    double login_attempted_time_;

    std::deque<Snapshot> snapshots_;
    double interpolation_offset_;
    double smoothed_snapshot_latency_;
    double snapshot_updated_time_;
};

#endif  // PLAYER_HPP
