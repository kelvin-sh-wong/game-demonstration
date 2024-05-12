#ifndef CONTACT_GENERATOR_HPP
#define CONTACT_GENERATOR_HPP

#include "registry.hpp"

class ContactGenerator {
public:
    ContactGenerator();

    void generate_contacts(Registry & registry) const;
};

#endif  // CONTACT_GENERATOR_HPP
