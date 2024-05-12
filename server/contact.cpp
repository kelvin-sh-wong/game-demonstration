#include "contact.hpp"

#include "vector2.hpp"

#include <cassert>
#include <cstdint>
#include <utility>

Contact::Contact(std::pair<std::uint32_t, std::uint32_t> entity_ids, Vector2 position, Vector2 penetration) : entity_ids_{std::move(entity_ids)}, position_{std::move(position)}, penetration_{std::move(penetration)} {
    assert(entity_ids_.first != 0);
    assert(entity_ids_.second != 0);
    assert(entity_ids_.first != entity_ids_.second);
    assert(length(penetration_) > 0.0);
}

std::pair<std::uint32_t, std::uint32_t> const & Contact::entity_ids() const {
    return entity_ids_;
}

Vector2 const & Contact::position() const {
    return position_;
}

Vector2 const & Contact::penetration() const {
    return penetration_;
}
