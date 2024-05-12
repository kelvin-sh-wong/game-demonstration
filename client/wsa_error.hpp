#ifndef WSA_ERROR_HPP
#define WSA_ERROR_HPP

#include <stdexcept>
#include <string>

class WSAError : public std::runtime_error {
public:
    WSAError(std::string const & message, int code);

    int code() const;

private:
    int code_;
};

#endif  // WSA_ERROR_HPP
