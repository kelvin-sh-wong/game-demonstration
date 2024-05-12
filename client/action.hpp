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
#include "serialize.hpp"
#include "vector2.hpp"

#include <ostream>
#include <vector>

template <>
struct serialize<Action> {
    void operator()(std::ostream & ostream, Action const & action) const {
        serialize<ActionType>{}(ostream, action.type());
        serialize<std::vector<Vector2>>{}(ostream, action.values());
    }
};

#endif  // ACTION_HPP
