#ifndef CONVEX_POLYGON_HPP
#define CONVEX_POLYGON_HPP

#include "vector2.hpp"

#include <vector>

class ConvexPolygon {
public:
    ConvexPolygon(std::vector<Vector2> vertices);

    std::vector<Vector2> const & vertices() const;

    void translate(Vector2 const & value);
    void rotate(double value);

private:
    std::vector<Vector2> vertices_;
};

#include "serialize.hpp"
#include "vector2.hpp"

#include <ostream>
#include <vector>

template <>
struct serialize<ConvexPolygon> {
    void operator()(std::ostream & ostream, ConvexPolygon const & convex_polygon) const {
        serialize<std::vector<Vector2>>{}(ostream, convex_polygon.vertices());
    }
};

#endif  // CONVEX_POLYGON_HPP
