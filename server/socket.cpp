#include "socket.hpp"

#include "address.hpp"
#include "wsa_error.hpp"

#include <winerror.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cassert>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

namespace {

std::string node(sockaddr const * address) {
    assert(address != nullptr);
    assert(address->sa_family == AF_INET || address->sa_family == AF_INET6);

    if (address->sa_family == AF_INET) {
        char ip_address[INET_ADDRSTRLEN];
        inet_ntop(address->sa_family, &(reinterpret_cast<sockaddr_in const *>(address)->sin_addr), ip_address, sizeof(ip_address));

        return ip_address;
    }
    else if (address->sa_family == AF_INET6) {
        char ip_address[INET6_ADDRSTRLEN];
        inet_ntop(address->sa_family, &(reinterpret_cast<sockaddr_in6 const *>(address)->sin6_addr), ip_address, sizeof(ip_address));

        return ip_address;
    }
    else {
        __assume(0);
    }
}

std::string service(sockaddr const * address) {
    assert(address != nullptr);
    assert(address->sa_family == AF_INET || address->sa_family == AF_INET6);

    if (address->sa_family == AF_INET) {
        unsigned short port_number;
        port_number = ntohs(reinterpret_cast<sockaddr_in const *>(address)->sin_port);

        return std::to_string(port_number);
    }
    else if (address->sa_family == AF_INET6) {
        unsigned short port_number;
        port_number = ntohs(reinterpret_cast<sockaddr_in6 const *>(address)->sin6_port);

        return std::to_string(port_number);
    }
    else {
        __assume(0);
    }
}

}

Socket::Socket(Address const & address) {
    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if ((socket_ = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol)) == INVALID_SOCKET) {
        throw WSAError{"Failed to create a socket.", WSAGetLastError()};
    }

    u_long non_blocking{1};
    if (ioctlsocket(socket_, FIONBIO, &non_blocking) != 0) {
        closesocket(socket_);

        throw WSAError{"Failed to set a socket to non-blocking mode.", WSAGetLastError()};
    }

    addrinfo * address_info;
    if (getaddrinfo(address.node().data(), address.service().data(), &hints, &address_info) != 0) {
        closesocket(socket_);

        throw WSAError{"Failed to get address information.", WSAGetLastError()};
    }

    if (bind(socket_, address_info->ai_addr, static_cast<int>(address_info->ai_addrlen)) == -1) {
        freeaddrinfo(address_info);
        closesocket(socket_);

        throw WSAError{"Failed to bind a socket to an address.", WSAGetLastError()};
    }

    freeaddrinfo(address_info);
}

Socket::~Socket() noexcept {
    closesocket(socket_);
}

void Socket::send(Address const & address, std::string const & message) const {
    assert(message.size() <= max_message_size);

    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    addrinfo * address_info;
    if (getaddrinfo(address.node().data(), address.service().data(), &hints, &address_info) != 0) {
        throw WSAError{"Failed to get address information.", WSAGetLastError()};
    }

    int byte_count;
    if ((byte_count = sendto(socket_, message.data(), static_cast<int>(message.size()), 0, address_info->ai_addr, static_cast<int>(address_info->ai_addrlen))) == -1) {
        freeaddrinfo(address_info);

        throw WSAError{"Failed to send a message.", WSAGetLastError()};
    }

    freeaddrinfo(address_info);
}

std::vector<std::pair<Address, std::string>> Socket::receive() const {
    std::vector<std::pair<Address, std::string>> packets;

    while (true) {
        char message[max_message_size];

        sockaddr_storage address;
        std::memset(&address, 0, sizeof(address));

        int address_size;
        address_size = sizeof(address);

        int byte_count;
        if ((byte_count = recvfrom(socket_, message, max_message_size, 0, reinterpret_cast<sockaddr *>(&address), &address_size)) == -1) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                break;
            }
            else if (WSAGetLastError() == WSAEMSGSIZE) {
                continue;
            }
            else if (WSAGetLastError() == WSAENETRESET) {
                continue;
            }
            else if (WSAGetLastError() == WSAECONNRESET) {
                continue;
            }
            else {
                throw WSAError{"Failed to receive a message.", WSAGetLastError()};
            }
        }

        packets.push_back(std::make_pair(Address{node(reinterpret_cast<sockaddr *>(&address)), service(reinterpret_cast<sockaddr *>(&address))}, std::string{message, static_cast<unsigned int>(byte_count)}));
    }

    return packets;
}
