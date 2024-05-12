#ifndef MESSAGE_TYPE_HPP
#define MESSAGE_TYPE_HPP

#include <cstdint>

enum class MessageType : std::uint8_t {
    null,
    login,
    login_response,
    snapshot,
    action
};

#include "serialize.hpp"

#include <cstdint>
#include <ostream>

template <>
struct serialize<MessageType> {
    void operator()(std::ostream & ostream, MessageType value) const {
        serialize<std::int8_t>{}(ostream, static_cast<std::int8_t>(value));
    }
};

#include "deserialize.hpp"

#include <cstdint>
#include <istream>

template <>
struct deserialize<MessageType> {
    MessageType operator()(std::istream & istream) const {
        return static_cast<MessageType>(deserialize<std::int8_t>{}(istream));
    }
};

#endif  // MESSAGE_TYPE_HPP
