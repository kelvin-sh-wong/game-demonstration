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

#include "deserialize.hpp"
#include "vector2.hpp"

#include <istream>
#include <utility>

template <>
struct deserialize<Transformation> {
    Transformation operator()(std::istream & istream) const {
        Vector2 position{deserialize<Vector2>{}(istream)};
        double rotation{deserialize<float>{}(istream)};

        return Transformation{std::move(position), rotation};
    }
};

#endif  // TRANSFORMATION_HPP
