#include "player.hpp"

#include "action.hpp"
#include "action_type.hpp"
#include "address.hpp"

#include <cstdint>
#include <unordered_map>
#include <utility>

Player::Player(Address address) : address_{std::move(address)}, entity_id_{0} {
    // empty
}

std::uint32_t Player::entity_id() const {
    return entity_id_;
}

void Player::set_entity_id(std::uint32_t entity_id) {
    entity_id_ = entity_id;
}

std::unordered_map<ActionType, Action> const & Player::actions() const {
    return actions_;
}

void Player::set_action(Action action) {
    ActionType type{action.type()};

    if (actions_.find(type) == actions_.end()) {
        actions_.insert(std::make_pair(type, std::move(action)));
    }
    else {
        actions_.at(type) = std::move(action);
    }
}

void Player::reset_action(ActionType type) {
    actions_.erase(type);
}
