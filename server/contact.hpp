#ifndef CONTACT_HPP
#define CONTACT_HPP

#include "vector2.hpp"

#include <cstdint>
#include <utility>

class Contact {
public:
    Contact(std::pair<std::uint32_t, std::uint32_t> entity_ids, Vector2 position, Vector2 penetration);

    std::pair<std::uint32_t, std::uint32_t> const & entity_ids() const;
    Vector2 const & position() const;
    Vector2 const & penetration() const;

private:
    std::pair<std::uint32_t, std::uint32_t> entity_ids_;
    Vector2 position_;
    Vector2 penetration_;
};

#endif  // CONTACT_HPP
