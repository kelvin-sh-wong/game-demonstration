#include "transformation.hpp"

#include "vector2.hpp"

#include <utility>

Transformation::Transformation(Vector2 position, double rotation) : position_{std::move(position)}, rotation_{rotation} {
    // empty
}

Vector2 const & Transformation::position() const {
    return position_;
}

void Transformation::set_position(Vector2 position) {
    position_ = std::move(position);
}

double Transformation::rotation() const {
    return rotation_;
}

void Transformation::set_rotation(double rotation) {
    rotation_ = rotation;
}
