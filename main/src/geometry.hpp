#pragma once
#include <tuple>
#include "main.h"
#include "util.hpp"
#include "drive.hpp"
#include "pid.hpp"
using namespace pros;
using namespace std;

enum class coord_types {VECTOR, POLAR};

class Point {

    double x, y, magnitude, direction;

public:
    // "constructors"

    Point(const double param_1, const double param_2, coord_types type = coord_types::VECTOR);

    // configures point as cartesian vector, with x and y coordinates
    void set_vector(const double x, const double y);

    // configures point as polar vector, with an angle and magnitude
    void set_polar(const double magnitude, const double direction);

    // getters
    double get_x() const;
    double get_y() const;
    double get_magnitude() const;
    double get_direction() const;

    // arithemtic operator overloads
    Point operator +(const Point& p2) const;
    Point operator -(const Point& p2) const;

};


class Line {

    Point p1, p2;
    double angle, length;

public:

    Line(Point p1, Point p2);

    // getters
    double get_angle() const;
    double get_length() const;
    Point get_point(short point = 1) const;

    // setters
    void set_angle(double angle);
    void set_length(double length);
};
