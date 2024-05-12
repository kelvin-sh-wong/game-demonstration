#ifndef VECTOR2_HPP
#define VECTOR2_HPP

class Vector2 {
public:
    Vector2();
    Vector2(double x, double y);

    Vector2 & operator+=(Vector2 const & vector);
    Vector2 & operator-=(Vector2 const & vector);
    Vector2 & operator*=(double scalar);

    double x;
    double y;
};

Vector2 operator+(Vector2 const & vector_1, Vector2 const & vector_2);
Vector2 operator-(Vector2 const & vector_1, Vector2 const & vector_2);
Vector2 operator-(Vector2 const & vector);
Vector2 operator*(double scalar, Vector2 const & vector);
Vector2 operator*(Vector2 const & vector, double scalar);
bool operator==(Vector2 const & vector_1, Vector2 const & vector_2);
bool operator!=(Vector2 const & vector_1, Vector2 const & vector_2);

double dot(Vector2 const & vector_1, Vector2 const & vector_2);
double wedge(Vector2 const & vector_1, Vector2 const & vector_2);

double length(Vector2 const & vector);
Vector2 normalize(Vector2 const & vector);

#include "serialize.hpp"

#include <ostream>

template <>
struct serialize<Vector2> {
    void operator()(std::ostream & ostream, Vector2 const & vector) const {
        serialize<float>{}(ostream, static_cast<float>(vector.x));
        serialize<float>{}(ostream, static_cast<float>(vector.y));
    }
};

#include "deserialize.hpp"

#include <istream>

template <>
struct deserialize<Vector2> {
    Vector2 operator()(std::istream & istream) const {
        double x{deserialize<float>{}(istream)};
        double y{deserialize<float>{}(istream)};

        return Vector2{x, y};
    }
};

#endif  // VECTOR2_HPP
