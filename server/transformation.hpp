#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include "vector2.hpp"

class Transformation {
public:
    Transformation(Vector2 position, double rotation);

    Vector2 const & position() const;
    void set_position(Vector2 position);

    double rotation() const;
    void set_rotation(double rotation);

private:
    Vector2 position_;
    double rotation_;
};

#include "serialize.hpp"
#include "vector2.hpp"

#include <ostream>

template <>
struct serialize<Transformation> {
    void operator()(std::ostream & ostream, Transformation const & transformation) const {
        serialize<Vector2>{}(ostream, transformation.position());
        serialize<float>{}(ostream, static_cast<float>(transformation.rotation()));
    }
};

#endif  // TRANSFORMATION_HPP
