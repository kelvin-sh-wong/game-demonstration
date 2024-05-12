#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "convex_polygon.hpp"

#include <variant>

class Collider {
public:
    Collider(ConvexPolygon convex_polygon);

    std::variant<ConvexPolygon> const & shape() const;
    void set_shape(ConvexPolygon convex_polygon);

private:
    std::variant<ConvexPolygon> shape_;
};

#include "convex_polygon.hpp"
#include "deserialize.hpp"

#include <cstdint>
#include <istream>
#include <utility>

template <>
struct deserialize<Collider> {
    Collider operator()(std::istream & istream) const {
        std::uint8_t shape_index{deserialize<std::uint8_t>{}(istream)};

        if (shape_index == 0) {
            ConvexPolygon convex_polygon{deserialize<ConvexPolygon>{}(istream)};

            return Collider{std::move(convex_polygon)};
        }
        else {
            __assume(0);
        }
    }
};

#endif  // COLLIDER_HPP
