#include "collider.hpp"

#include "contact.hpp"
#include "convex_polygon.hpp"

#include <utility>
#include <variant>
#include <vector>

Collider::Collider(ConvexPolygon convex_polygon) : shape_{std::move(convex_polygon)} {
    // empty
}

std::variant<ConvexPolygon> const & Collider::shape() const {
    return shape_;
}

void Collider::set_shape(ConvexPolygon convex_polygon) {
    shape_ = std::move(convex_polygon);
}

std::vector<Contact> const & Collider::contacts() const {
    return contacts_;
}

void Collider::add_contact(Contact contact) {
    contacts_.push_back(std::move(contact));
}

void Collider::clear_contacts() {
    contacts_.clear();
}
