#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "contact.hpp"
#include "convex_polygon.hpp"

#include <variant>
#include <vector>

class Collider {
public:
    Collider(ConvexPolygon convex_polygon);

    std::variant<ConvexPolygon> const & shape() const;
    void set_shape(ConvexPolygon convex_polygon);

    std::vector<Contact> const & contacts() const;
    void add_contact(Contact contact);
    void clear_contacts();

private:
    std::variant<ConvexPolygon> shape_;

    std::vector<Contact> contacts_;
};

#include "convex_polygon.hpp"
#include "serialize.hpp"

#include <cstdint>
#include <ostream>
#include <variant>

template <>
struct serialize<Collider> {
    void operator()(std::ostream & ostream, Collider const & collider) const {
        std::uint8_t shape_index;

        if (std::holds_alternative<ConvexPolygon>(collider.shape())) {
            shape_index = 0;
        }
        else {
            __assume(0);
        }

        serialize<std::uint8_t>{}(ostream, shape_index);

        if (std::holds_alternative<ConvexPolygon>(collider.shape())) {
            serialize<ConvexPolygon>{}(ostream, std::get<ConvexPolygon>(collider.shape()));
        }
        else {
            __assume(0);
        }
    }
};

#endif  // COLLIDER_HPP
