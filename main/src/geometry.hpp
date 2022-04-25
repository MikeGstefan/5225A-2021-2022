#pragma once
#include "main.h"

class Vector; // forward declares Vector class
struct Position;

struct Point{
  double x, y;
  // arithmetic operator overloads
  Vector operator +(const Point& p2) const;
  Vector operator -(const Point& p2) const;
  
  Point();
  Point(Position position);
  Point(double x, double y);
};

struct Position{   // stores members variables for a coordinate: x, y and angle    MUST BE DEFINED HERE, OTHERWISE IT'LL CAUSE CIRCULAR HEADER FILE DEPENDENCY OCCURS
  double x, y, angle;
  Position(); // default constructor
  Position(Point point);
  Position(Point point, double angle);
  Position(double x, double y);
  Position(double x, double y, double angle);
};

enum class vector_types {CARTESIAN, POLAR};

class Vector{

    double x, y, magnitude, angle;

  public:
    // constructors
    Vector(const double param_1, const double param_2, vector_types type = vector_types::CARTESIAN);
    Vector(const Point point);

    // configures vector as cartesian vector, with x and y coordinates
    void set_cartesian(const double x, const double y);
    void set_cartesian(const Point point);

    // configures vector as polar vector, with an angle and magnitude
    void set_polar(const double magnitude, const double direction);

    void rotate(const double rotation_angle);

    // getters
    double get_x() const;
    double get_y() const;
    double get_magnitude() const;
    double get_angle() const;

    // arithmetic operator overloads
    Vector operator +(const Vector& p2) const;
    Vector operator -(const Vector& p2) const;

};
