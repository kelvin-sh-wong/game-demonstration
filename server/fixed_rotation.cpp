#include "fixed_rotation.hpp"

FixedRotation::FixedRotation(double value) : value_{value} {
    // empty
}

double FixedRotation::value() const {
    return value_;
}

void FixedRotation::set_value(double value) {
    value_ = value;
}
