#include "glad_error.hpp"

#include <stdexcept>
#include <string>

GLADError::GLADError(std::string const & message) : std::runtime_error{message} {
    // empty
}
