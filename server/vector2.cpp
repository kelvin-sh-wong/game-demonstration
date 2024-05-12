#include "vector2.hpp"

#include <cassert>
#include <cmath>

Vector2::Vector2() : Vector2{0.0, 0.0} {
    // empty
}

Vector2::Vector2(double x, double y) : x{x}, y{y} {
    // empty
}

Vector2 & Vector2::operator+=(Vector2 const & vector) {
    x += vector.x;
    y += vector.y;

    return *this;
}

Vector2 & Vector2::operator-=(Vector2 const & vector) {
    x -= vector.x;
    y -= vector.y;

    return *this;
}

Vector2 & Vector2::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;

    return *this;
}

Vector2 operator+(Vector2 const & vector_1, Vector2 const & vector_2) {
    return Vector2{vector_1} += vector_2;
}

Vector2 operator-(Vector2 const & vector_1, Vector2 const & vector_2) {
    return Vector2{vector_1} -= vector_2;
}

Vector2 operator-(Vector2 const & vector) {
    return -1.0 * vector;
}

Vector2 operator*(double scalar, Vector2 const & vector) {
    return Vector2{vector} *= scalar;
}

Vector2 operator*(Vector2 const & vector, double scalar) {
    return scalar * vector;
}

bool operator==(Vector2 const & vector_1, Vector2 const & vector_2) {
    return vector_1.x == vector_2.x && vector_1.y == vector_2.y;
}

bool operator!=(Vector2 const & vector_1, Vector2 const & vector_2) {
    return !(vector_1 == vector_2);
}

double dot(Vector2 const & vector_1, Vector2 const & vector_2) {
    return vector_1.x * vector_2.x + vector_1.y * vector_2.y;
}

double wedge(Vector2 const & vector_1, Vector2 const & vector_2) {
    return vector_1.x * vector_2.y - vector_1.y * vector_2.x;
}

double length(Vector2 const & vector) {
    return std::sqrt(dot(vector, vector));
}

Vector2 normalize(Vector2 const & vector) {
    assert(length(vector) != 0.0);

    return vector * (1.0 / length(vector));
}
