#pragma once
#include <tuple>
#include "main.h"
#include "util.hpp"
#include "drive.hpp"
#include "pid.hpp"
using namespace pros;
using namespace std;

enum class coord_types {VECTOR, POLAR};

class point {

    double x, y, magnitude, direction;

public:
    // "constructors"

    point(const double param_1, const double param_2, coord_types type = coord_types::VECTOR);

    // configures point as cartesian vector, with x and y coordinates
    void set_vector(const double x, const double y);

    // configures point as polar vector, with an angle and magnitude
    void set_polar(const double magnitude, const double direction);

    // getters
    double get_x();
    double get_y();
    double get_magnitude();
    double get_direction();

    // arithemtic operator overloads
    point operator +(const point& p2);
    point operator -(const point& p2);

};


class line {

    point p1, p2;
    double angle, length;

public:

    line(point p1, point p2);

    // getters
    double get_angle();
    double get_length();
    point get_point(short point = 1);

    // setters
    void set_angle(double angle);
    void set_length(double length);
    void set_x_of_point(double x, short point);
    void set_y_of_point(double y, short point);

};
