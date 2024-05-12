#include "action.hpp"

#include "action_type.hpp"
#include "vector2.hpp"

#include <utility>
#include <vector>

Action::Action(ActionType type, std::vector<Vector2> values) : type_{type}, values_{std::move(values)} {
    // empty
}

ActionType Action::type() const {
    return type_;
}

std::vector<Vector2> const & Action::values() const {
    return values_;
}
