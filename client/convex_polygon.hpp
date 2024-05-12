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

#include "deserialize.hpp"
#include "vector2.hpp"

#include <istream>
#include <utility>
#include <vector>

template <>
struct deserialize<ConvexPolygon> {
    ConvexPolygon operator()(std::istream & istream) const {
        std::vector<Vector2> vertices{deserialize<std::vector<Vector2>>{}(istream)};

        return ConvexPolygon{std::move(vertices)};
    }
};

#endif  // CONVEX_POLYGON_HPP
