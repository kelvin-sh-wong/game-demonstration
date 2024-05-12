#ifndef GLAD_ERROR_HPP
#define GLAD_ERROR_HPP

#include <stdexcept>
#include <string>

class GLADError : public std::runtime_error {
public:
    GLADError(std::string const & message);
};

#endif  // GLAD_ERROR_HPP
