#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "vector2.hpp"

class Movement {
public:
    Movement();

    Vector2 const & direction() const;
    void set_direction(Vector2 const & direction);

    double speed() const;
    void set_speed(double speed);

private:
    Vector2 direction_;
    double speed_;
};

#endif  // MOVEMENT_HPP
