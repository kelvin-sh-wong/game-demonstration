#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

class RigidBody {
public:
    RigidBody(double mass);

    double mass() const;
    void set_mass(double mass);

private:
    double mass_;
};

#endif  // RIGID_BODY_HPP
