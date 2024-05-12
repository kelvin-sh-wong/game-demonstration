#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "address.hpp"

#include <winsock2.h>

#include <string>
#include <utility>
#include <vector>

class Socket {
public:
    Socket(Address const & address);
    Socket(Socket const &) = delete;
    Socket & operator=(Socket const &) = delete;
    ~Socket() noexcept;

    void send(Address const & address, std::string const & message) const;

    std::vector<std::pair<Address, std::string>> receive() const;

private:
    static constexpr int max_message_size{512};

    SOCKET socket_;
};

#endif  // SOCKET_HPP
