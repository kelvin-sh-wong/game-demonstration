#include "motion.hpp"

#include "vector2.hpp"

#include <utility>

Motion::Motion() : linear_velocity_{Vector2{0.0, 0.0}}, angular_velocity_{0.0} {
    // empty
}

Vector2 const & Motion::linear_velocity() const {
    return linear_velocity_;
}

void Motion::set_linear_velocity(Vector2 linear_velocity) {
    linear_velocity_ = std::move(linear_velocity);
}

double Motion::angular_velocity() const {
    return angular_velocity_;
}

void Motion::set_angular_velocity(double angular_velocity) {
    angular_velocity_ = angular_velocity;
}
