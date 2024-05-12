#ifndef ACTION_TYPE_HPP
#define ACTION_TYPE_HPP

#include <cstdint>

enum class ActionType : std::uint8_t {
    null,
    movement
};

#include "deserialize.hpp"

#include <cstdint>
#include <istream>

template <>
struct deserialize<ActionType> {
    ActionType operator()(std::istream & istream) const {
        return static_cast<ActionType>(deserialize<std::int8_t>{}(istream));
    }
};

#endif  // ACTION_TYPE_HPP
