#ifndef MOTION_HPP
#define MOTION_HPP

#include "vector2.hpp"

class Motion {
public:
    Motion();

    Vector2 const & linear_velocity() const;
    void set_linear_velocity(Vector2 linear_velocity);

    double angular_velocity() const;
    void set_angular_velocity(double angular_velocity);

private:
    Vector2 linear_velocity_;
    double angular_velocity_;
};

#endif  // MOTION_HPP
