#include "address.hpp"

#include <string>
#include <utility>

Address::Address(std::string node, std::string service) : node_{std::move(node)}, service_{std::move(service)} {
    // empty
}

std::string const & Address::node() const {
    return node_;
}

std::string const & Address::service() const {
    return service_;
}

bool operator==(Address const & address_1, Address const & address_2) {
    return address_1.node() == address_2.node() && address_1.service() == address_2.service();
}

bool operator!=(Address const & address_1, Address const & address_2) {
    return !(address_1 == address_2);
}
