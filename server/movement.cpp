#include "movement.hpp"

#include "vector2.hpp"

#include <cassert>

Movement::Movement() : direction_{Vector2{0.0, 0.0}}, speed_{0.0} {
    // empty
}

Vector2 const & Movement::direction() const {
    return direction_;
}

void Movement::set_direction(Vector2 const & direction) {
    if (length(direction) == 0.0) {
        direction_ = Vector2{0.0, 0.0};
    }
    else {
        direction_ = normalize(direction);
    }
}

double Movement::speed() const {
    return speed_;
}

void Movement::set_speed(double speed) {
    assert(speed >= 0.0);

    speed_ = speed;
}
