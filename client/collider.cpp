#include "collider.hpp"

#include "convex_polygon.hpp"

#include <utility>
#include <variant>

Collider::Collider(ConvexPolygon convex_polygon) : shape_{std::move(convex_polygon)} {
    // empty
}

std::variant<ConvexPolygon> const & Collider::shape() const {
    return shape_;
}

void Collider::set_shape(ConvexPolygon convex_polygon) {
    shape_ = std::move(convex_polygon);
}
