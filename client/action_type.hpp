#ifndef ACTION_TYPE_HPP
#define ACTION_TYPE_HPP

#include <cstdint>

enum class ActionType : std::uint8_t {
    null,
    movement
};

#include "serialize.hpp"

#include <cstdint>
#include <ostream>

template <>
struct serialize<ActionType> {
    void operator()(std::ostream & ostream, ActionType value) const {
        serialize<std::int8_t>{}(ostream, static_cast<std::int8_t>(value));
    }
};

#endif  // ACTION_TYPE_HPP
