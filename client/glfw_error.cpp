#include "glfw_error.hpp"

#include <stdexcept>
#include <string>

GLFWError::GLFWError(std::string const & message, int code) : std::runtime_error{message}, code_{code} {
    // empty
}

int GLFWError::code() const {
    return code_;
}
