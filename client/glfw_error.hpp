#ifndef GLFW_ERROR_HPP
#define GLFW_ERROR_HPP

#include <stdexcept>
#include <string>

class GLFWError : public std::runtime_error {
public:
    GLFWError(std::string const & message, int code);

    int code() const;

private:
    int code_;
};

#endif  // GLFW_ERROR_HPP
