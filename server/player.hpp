#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "action.hpp"
#include "action_type.hpp"
#include "address.hpp"

#include <cstdint>
#include <unordered_map>

class Player {
public:
    Player(Address address);

    std::uint32_t entity_id() const;
    void set_entity_id(std::uint32_t entity_id);

    std::unordered_map<ActionType, Action> const & actions() const;
    void set_action(Action action);
    void reset_action(ActionType type);

private:
    Address address_;

    std::uint32_t entity_id_;

    std::unordered_map<ActionType, Action> actions_;
};

#endif  // PLAYER_HPP
