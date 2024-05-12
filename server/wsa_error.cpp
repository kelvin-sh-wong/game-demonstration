#include "wsa_error.hpp"

#include <stdexcept>
#include <string>

WSAError::WSAError(std::string const & message, int code) : std::runtime_error{message}, code_{code} {
    // empty
}

int WSAError::code() const {
    return code_;
}
