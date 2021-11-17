#pragma once
#include "main.h"

struct Point{
    double x, y;
};

struct Position{   // stores members variables for a coordinate: x, y and angle    MUST BE DEFINED HERE, OTHERWISE IT'LL CAUSE CIRCULAR HEADER FILE DEPENDENCY OCCURS
    double x, y, angle;
    Position(double x, double y, double angle);
    Position();
    // default constructor
};

enum class vector_types {CARTESIAN, POLAR};

class Vector{

    double x, y, magnitude, angle;

public:
    // "constructors"

    Vector(const double param_1, const double param_2, vector_types type = vector_types::CARTESIAN);

    // configures point as cartesian vector, with x and y coordinates
    void set_cartesian(const double x, const double y);

    // configures point as polar vector, with an angle and magnitude
    void set_polar(const double magnitude, const double direction);

    // getters
    double get_x() const;
    double get_y() const;
    double get_magnitude() const;
    double get_angle() const;

    // arithmetic operator overloads
    Vector operator +(const Vector& p2) const;
    Vector operator -(const Vector& p2) const;

};
