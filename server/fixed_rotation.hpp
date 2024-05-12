#ifndef FIXED_ROTATION_HPP
#define FIXED_ROTATION_HPP

class FixedRotation {
public:
    FixedRotation(double value);

    double value() const;
    void set_value(double value);

private:
    double value_;
};

#endif  // FIXED_ROTATION_HPP
