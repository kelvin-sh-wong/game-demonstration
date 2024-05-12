#ifndef ACTION_HPP
#define ACTION_HPP

#include "action_type.hpp"
#include "vector2.hpp"

#include <vector>

class Action {
public:
    Action(ActionType type, std::vector<Vector2> values);

    ActionType type() const;
    std::vector<Vector2> const & values() const;

private:
    ActionType type_;
    std::vector<Vector2> values_;
};

#include "action_type.hpp"
#include "deserialize.hpp"
#include "vector2.hpp"

#include <istream>
#include <utility>
#include <vector>

template <>
struct deserialize<Action> {
    Action operator()(std::istream & istream) const {
        ActionType type{deserialize<ActionType>{}(istream)};
        std::vector<Vector2> values{deserialize<std::vector<Vector2>>{}(istream)};

        return Action{type, std::move(values)};
    }
};

#endif  // ACTION_HPP
