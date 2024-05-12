#include "convex_polygon.hpp"

#include "vector2.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

bool is_convex_polygon(std::vector<Vector2> const & vertices) {
    if (vertices.size() < 3) {
        return false;
    }

    for (std::size_t i{0}; i < vertices.size(); ++i) {
        Vector2 vector_1{vertices[(i + 1) % vertices.size()] - vertices[i]};
        Vector2 vector_2{vertices[(i - 1 + vertices.size()) % vertices.size()] - vertices[i]};

        if (wedge(vector_1, vector_2) <= 0.0) {
            return false;
        }
    }

    return true;
}

}

ConvexPolygon::ConvexPolygon(std::vector<Vector2> vertices) : vertices_{std::move(vertices)} {
    assert(is_convex_polygon(vertices_));
}

std::vector<Vector2> const & ConvexPolygon::vertices() const {
    return vertices_;
}

void ConvexPolygon::translate(Vector2 const & value) {
    for (Vector2 & vertex : vertices_) {
        vertex += value;
    }
}

void ConvexPolygon::rotate(double value) {
    for (Vector2 & vertex : vertices_) {
        vertex = Vector2{vertex.x * std::cos(value) - vertex.y * std::sin(value), vertex.x * std::sin(value) + vertex.y * std::cos(value)};
    }
}
