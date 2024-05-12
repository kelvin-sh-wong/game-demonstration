#include "contact_generator.hpp"

#include "collider.hpp"
#include "contact.hpp"
#include "convex_polygon.hpp"
#include "entity.hpp"
#include "registry.hpp"
#include "transformation.hpp"
#include "vector2.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <variant>
#include <vector>

namespace {

Vector2 get_support(ConvexPolygon const & convex_polygon, Vector2 const & axis) {
    assert(length(axis) != 0.0);

    Vector2 support;

    double max_projection{-std::numeric_limits<double>::infinity()};

    for (Vector2 const & vertex : convex_polygon.vertices()) {
        double projection{dot(vertex, axis)};

        if (projection > max_projection) {
            max_projection = projection;

            support = vertex;
        }
    }

    return support;
}

double get_distance(Vector2 const & point, std::pair<Vector2, Vector2> const & line) {
    assert(line.first != line.second);

    return dot(point - line.first, normalize(Vector2{line.second.y - line.first.y, line.first.x - line.second.x}));
}

Vector2 get_intersection(std::pair<Vector2, Vector2> const & line_1, std::pair<Vector2, Vector2> const & line_2) {
    double a_1{line_1.second.y - line_1.first.y};
    double b_1{line_1.first.x - line_1.second.x};
    double c_1{a_1 * line_1.first.x + b_1 * line_1.first.y};

    double a_2{line_2.second.y - line_2.first.y};
    double b_2{line_2.first.x - line_2.second.x};
    double c_2{a_2 * line_2.first.x + b_2 * line_2.first.y};

    assert(a_1 * b_2 - a_2 * b_1 != 0.0);

    double x{(c_1 * b_2 - c_2 * b_1) / (a_1 * b_2 - a_2 * b_1)};
    double y{(a_1 * c_2 - a_2 * c_1) / (a_1 * b_2 - a_2 * b_1)};

    return Vector2{x, y};
}

std::vector<std::pair<Vector2, Vector2>> get_contacts(ConvexPolygon const & convex_polygon_1, ConvexPolygon const & convex_polygon_2) {
    std::vector<std::pair<Vector2, Vector2>> contacts;

    double min_penetration_depth{std::numeric_limits<double>::infinity()};

    std::vector<ConvexPolygon const *> convex_polygons{&convex_polygon_1, &convex_polygon_2};

    for (std::size_t convex_polygon_index{0}; convex_polygon_index < convex_polygons.size(); ++convex_polygon_index) {
        ConvexPolygon const & reference_convex_polygon{*convex_polygons[convex_polygon_index]};
        ConvexPolygon const & incident_convex_polygon{*convex_polygons[(convex_polygon_index + 1) % convex_polygons.size()]};

        for (std::size_t vertex_index{0}; vertex_index < reference_convex_polygon.vertices().size(); ++vertex_index) {
            std::pair<Vector2, Vector2> reference_edge;
            {
                reference_edge.first = reference_convex_polygon.vertices()[vertex_index];
                reference_edge.second = reference_convex_polygon.vertices()[(vertex_index + 1) % reference_convex_polygon.vertices().size()];
            }

            Vector2 contact_normal{normalize(Vector2{reference_edge.second.y - reference_edge.first.y, reference_edge.first.x - reference_edge.second.x})};

            std::pair<Vector2, Vector2> incident_edge;
            {
                std::vector<Vector2> const & vertices{incident_convex_polygon.vertices()};

                incident_edge.first = get_support(incident_convex_polygon, -contact_normal);

                std::size_t support_index;
                for (support_index = 0; support_index < vertices.size(); ++support_index) {
                    if (incident_edge.first == vertices[support_index]) {
                        break;
                    }
                }

                double min_alignment{std::numeric_limits<double>::infinity()};

                for (std::size_t adjacent_index : {(support_index + 1) % vertices.size(), (support_index - 1 + vertices.size()) % vertices.size()}) {
                    double ratio{dot(normalize(vertices[adjacent_index] - vertices[support_index]), contact_normal)};

                    if (ratio < min_alignment) {
                        min_alignment = ratio;

                        incident_edge.second = vertices[adjacent_index];
                    }
                }
            }

            std::vector<std::pair<Vector2, Vector2>> clipping_edges;
            {
                clipping_edges.push_back(std::make_pair(reference_edge.second, reference_edge.second - contact_normal));
                clipping_edges.push_back(std::make_pair(reference_edge.first - contact_normal, reference_edge.first));
            }

            std::vector<Vector2> positions;
            std::vector<Vector2> penetrations;

            for (Vector2 position : {incident_edge.first, incident_edge.second}) {
                for (std::pair<Vector2, Vector2> const & clipping_edge : clipping_edges) {
                    if (get_distance(position, clipping_edge) <= 0.0) {
                        continue;
                    }

                    position = get_intersection(incident_edge, clipping_edge);

                    break;
                }

                double penetration_depth{-get_distance(position, reference_edge)};

                if (penetration_depth <= 0.0) {
                    break;
                }

                positions.push_back(std::move(position));
                penetrations.push_back(-contact_normal * penetration_depth);
            }

            if (positions.size() == 0) {
                return std::vector<std::pair<Vector2, Vector2>>{};
            }

            if (length(penetrations[0]) >= min_penetration_depth) {
                continue;
            }

            min_penetration_depth = length(penetrations[0]);

            contacts.clear();

            for (std::size_t i{0}; i < positions.size(); ++i) {
                if (&reference_convex_polygon == &convex_polygon_1) {
                    contacts.push_back(std::make_pair(positions[i] - penetrations[i], penetrations[i]));
                }
                else if (&reference_convex_polygon == &convex_polygon_2) {
                    contacts.push_back(std::make_pair(positions[i], -penetrations[i]));
                }
                else {
                    __assume(0);
                }
            }
        }
    }

    return contacts;
}

}

ContactGenerator::ContactGenerator() {
    // empty
}

void ContactGenerator::generate_contacts(Registry & registry) const {
    for (Entity & entity : registry.entities) {
        if (entity.collider == nullptr) {
            continue;
        }

        Collider & collider{*entity.collider};

        collider.clear_contacts();
    }

    for (std::size_t entity_id_1{0}; entity_id_1 < registry.entities.size(); ++entity_id_1) {
        for (std::size_t entity_id_2{entity_id_1 + 1}; entity_id_2 < registry.entities.size(); ++entity_id_2) {
            Entity & entity_1{registry.entities[entity_id_1]};
            Entity & entity_2{registry.entities[entity_id_2]};

            if (entity_1.transformation == nullptr || entity_1.collider == nullptr) {
                continue;
            }
            if (entity_2.transformation == nullptr || entity_2.collider == nullptr) {
                continue;
            }

            Transformation const & transformation_1{*entity_1.transformation};
            Collider & collider_1{*entity_1.collider};

            Transformation const & transformation_2{*entity_2.transformation};
            Collider & collider_2{*entity_2.collider};

            if (std::holds_alternative<ConvexPolygon>(collider_1.shape()) && std::holds_alternative<ConvexPolygon>(collider_2.shape())) {
                ConvexPolygon convex_polygon_1{std::get<ConvexPolygon>(collider_1.shape())};
                convex_polygon_1.rotate(transformation_1.rotation());
                convex_polygon_1.translate(transformation_1.position());

                ConvexPolygon convex_polygon_2{std::get<ConvexPolygon>(collider_2.shape())};
                convex_polygon_2.rotate(transformation_2.rotation());
                convex_polygon_2.translate(transformation_2.position());

                for (auto const & [position, penetration] : get_contacts(convex_polygon_1, convex_polygon_2)) {
                    collider_1.add_contact(Contact{std::make_pair(static_cast<std::uint32_t>(entity_id_1), static_cast<std::uint32_t>(entity_id_2)), position, penetration});
                    collider_2.add_contact(Contact{std::make_pair(static_cast<std::uint32_t>(entity_id_2), static_cast<std::uint32_t>(entity_id_1)), position - penetration, -penetration});
                }
            }
            else {
                __assume(0);
            }
        }
    }
}
