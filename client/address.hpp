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

#endif  // ADDRESS_HPP
