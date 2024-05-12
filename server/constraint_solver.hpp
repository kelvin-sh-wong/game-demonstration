#ifndef CONSTRAINT_SOLVER_HPP
#define CONSTRAINT_SOLVER_HPP

#include "registry.hpp"

class ConstraintSolver {
public:
    ConstraintSolver();

    void solve_constraints(Registry & registry, double delta_time) const;
};

#endif  // CONSTRAINT_SOLVER_HPP
