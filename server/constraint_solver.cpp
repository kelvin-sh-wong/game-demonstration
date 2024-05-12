#include "constraint_solver.hpp"

#include "collider.hpp"
#include "contact.hpp"
#include "convex_polygon.hpp"
#include "entity.hpp"
#include "fixed_rotation.hpp"
#include "matrix.hpp"
#include "motion.hpp"
#include "registry.hpp"
#include "rigid_body.hpp"
#include "transformation.hpp"
#include "vector2.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace {

double get_mass_moment_of_inertia(double mass, ConvexPolygon const & convex_polygon) {
    double sum_1{0.0};
    double sum_2{0.0};

    for (std::size_t i{0}; i < convex_polygon.vertices().size(); ++i) {
        Vector2 vertex_1{convex_polygon.vertices()[i]};
        Vector2 vertex_2{convex_polygon.vertices()[(i + 1) % convex_polygon.vertices().size()]};

        sum_1 += wedge(vertex_2, vertex_1) * (dot(vertex_1, vertex_1) + dot(vertex_1, vertex_2) + dot(vertex_2, vertex_2));
        sum_2 += wedge(vertex_2, vertex_1);
    }

    return mass * (sum_1 / (6.0 * sum_2));
}

double get_mass_moment_of_inertia(double mass, std::variant<ConvexPolygon> const & shape) {
    if (std::holds_alternative<ConvexPolygon>(shape)) {
        return get_mass_moment_of_inertia(mass, std::get<ConvexPolygon>(shape));
    }
    else {
        __assume(0);
    }
}

}

ConstraintSolver::ConstraintSolver() {
    // empty
}

void ConstraintSolver::solve_constraints(Registry & registry, double delta_time) const {
    std::vector<Contact const *> contacts;

    for (Entity const & entity : registry.entities) {
        if (entity.collider == nullptr) {
            continue;
        }

        Collider const & collider{*entity.collider};

        for (Contact const & contact : collider.contacts()) {
            if (contact.entity_ids().first < contact.entity_ids().second) {
                contacts.push_back(&contact);
            }
        }
    }

    std::unordered_map<Contact const *, double> contact_normal_impulses;

    for (Contact const * contact : contacts) {
        contact_normal_impulses.insert(std::make_pair(contact, 0.0));
    }

    for (std::size_t iteration{0}; iteration < 16; ++iteration) {
        for (Contact const * contact : contacts) {
            Entity & entity_1{registry.entities[contact->entity_ids().first]};
            Entity & entity_2{registry.entities[contact->entity_ids().second]};

            if (entity_1.transformation == nullptr || entity_1.motion == nullptr || entity_1.collider == nullptr || entity_1.rigid_body == nullptr) {
                continue;
            }
            if (entity_2.transformation == nullptr || entity_2.motion == nullptr || entity_2.collider == nullptr || entity_2.rigid_body == nullptr) {
                continue;
            }

            Transformation const & transformation_1{*entity_1.transformation};
            Motion & motion_1{*entity_1.motion};
            Collider const & collider_1{*entity_1.collider};
            RigidBody const & rigid_body_1{*entity_1.rigid_body};

            Transformation const & transformation_2{*entity_2.transformation};
            Motion & motion_2{*entity_2.motion};
            Collider const & collider_2{*entity_2.collider};
            RigidBody const & rigid_body_2{*entity_2.rigid_body};

            if (std::isinf(rigid_body_1.mass()) && std::isinf(rigid_body_2.mass())) {
                continue;
            }

            Vector2 contact_normal{-normalize(contact->penetration())};

            Matrix<6, 6> inverse_mass{Matrix<6, 6>::zero()};
            inverse_mass[0][0] = 1.0 / rigid_body_1.mass();
            inverse_mass[1][1] = 1.0 / rigid_body_1.mass();
            inverse_mass[2][2] = 1.0 / get_mass_moment_of_inertia(rigid_body_1.mass(), collider_1.shape());
            inverse_mass[3][3] = 1.0 / rigid_body_2.mass();
            inverse_mass[4][4] = 1.0 / rigid_body_2.mass();
            inverse_mass[5][5] = 1.0 / get_mass_moment_of_inertia(rigid_body_2.mass(), collider_2.shape());

            Matrix<1, 6> jacobian{Matrix<1, 6>::zero()};
            jacobian[0][0] = -contact_normal.x;
            jacobian[0][1] = -contact_normal.y;
            jacobian[0][2] = -wedge(contact->position() - transformation_1.position(), contact_normal);
            jacobian[0][3] = contact_normal.x;
            jacobian[0][4] = contact_normal.y;
            jacobian[0][5] = wedge((contact->position() + contact->penetration()) - transformation_2.position(), contact_normal);

            Matrix<6, 1> velocity{Matrix<6, 1>::zero()};
            velocity[0][0] = motion_1.linear_velocity().x;
            velocity[1][0] = motion_1.linear_velocity().y;
            velocity[2][0] = motion_1.angular_velocity();
            velocity[3][0] = motion_2.linear_velocity().x;
            velocity[4][0] = motion_2.linear_velocity().y;
            velocity[5][0] = motion_2.angular_velocity();

            Matrix<1, 1> bias{Matrix<1, 1>::zero()};
            bias[0][0] = (0.1 / delta_time) * -std::max(length(contact->penetration()) - 1.0, 0.0);

            double effective_mass{1.0 / (jacobian * inverse_mass * transpose(jacobian))[0][0]};

            double lambda{effective_mass * (-(jacobian * velocity + bias))[0][0]};

            double & normal_impulse{contact_normal_impulses.at(contact)};
            lambda = std::clamp(normal_impulse + lambda, 0.0, std::numeric_limits<double>::infinity()) - normal_impulse;
            normal_impulse += lambda;

            Matrix<6, 1> delta_velocity{lambda * (inverse_mass * transpose(jacobian))};

            motion_1.set_linear_velocity(motion_1.linear_velocity() + Vector2{delta_velocity[0][0], delta_velocity[1][0]});
            motion_1.set_angular_velocity(motion_1.angular_velocity() + delta_velocity[2][0]);
            motion_2.set_linear_velocity(motion_2.linear_velocity() + Vector2{delta_velocity[3][0], delta_velocity[4][0]});
            motion_2.set_angular_velocity(motion_2.angular_velocity() + delta_velocity[5][0]);
        }

        for (Entity & entity : registry.entities) {
            if (entity.transformation == nullptr || entity.motion == nullptr || entity.collider == nullptr || entity.rigid_body == nullptr || entity.fixed_rotation == nullptr) {
                continue;
            }

            Transformation const & transformation{*entity.transformation};
            Motion & motion{*entity.motion};
            Collider const & collider{*entity.collider};
            RigidBody const & rigid_body{*entity.rigid_body};
            FixedRotation const & fixed_rotation{*entity.fixed_rotation};

            if (std::isinf(rigid_body.mass())) {
                continue;
            }

            Matrix<3, 3> inverse_mass{Matrix<3, 3>::zero()};
            inverse_mass[0][0] = 1.0 / rigid_body.mass();
            inverse_mass[1][1] = 1.0 / rigid_body.mass();
            inverse_mass[2][2] = 1.0 / get_mass_moment_of_inertia(rigid_body.mass(), collider.shape());

            Matrix<1, 3> jacobian{Matrix<1, 3>::zero()};
            jacobian[0][0] = 0.0;
            jacobian[0][1] = 0.0;
            jacobian[0][2] = 1.0;

            Matrix<3, 1> velocity{Matrix<3, 1>::zero()};
            velocity[0][0] = motion.linear_velocity().x;
            velocity[1][0] = motion.linear_velocity().y;
            velocity[2][0] = motion.angular_velocity();

            Matrix<1, 1> bias{Matrix<1, 1>::zero()};
            bias[0][0] = (0.1 / delta_time) * -(fixed_rotation.value() - transformation.rotation());

            double effective_mass{1.0 / (jacobian * inverse_mass * transpose(jacobian))[0][0]};

            double lambda{effective_mass * (-(jacobian * velocity + bias))[0][0]};

            Matrix<3, 1> delta_velocity{lambda * (inverse_mass * transpose(jacobian))};

            motion.set_linear_velocity(motion.linear_velocity() + Vector2{delta_velocity[0][0], delta_velocity[1][0]});
            motion.set_angular_velocity(motion.angular_velocity() + delta_velocity[2][0]);
        }
    }
}
