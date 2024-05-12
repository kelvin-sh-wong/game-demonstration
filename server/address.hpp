#ifndef ADDRESS_HPP
#define ADDRESS_HPP

#include <string>

class Address {
public:
    Address(std::string node, std::string service);

    std::string const & node() const;
    std::string const & service() const;

private:
    std::string node_;
    std::string service_;
};

bool operator==(Address const & address_1, Address const & address_2);
bool operator!=(Address const & address_1, Address const & address_2);

#include "hash_combine.hpp"

#include <cstddef>
#include <functional>

template <>
struct std::hash<Address> {
    std::size_t operator()(Address const & address) const noexcept {
        std::size_t seed{0};
        hash_combine(seed, address.node());
        hash_combine(seed, address.service());

        return seed;
    }
};

#endif  // ADDRESS_HPP
