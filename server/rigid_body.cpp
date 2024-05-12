#include "rigid_body.hpp"

#include <cassert>

RigidBody::RigidBody(double mass) : mass_{mass} {
    assert(mass_ > 0.0);
}

double RigidBody::mass() const {
    return mass_;
}

void RigidBody::set_mass(double mass) {
    assert(mass > 0.0);

    mass_ = mass;
}
