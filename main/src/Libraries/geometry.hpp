#pragma once
#include "main.h"

class Vector; // forward declares Vector class
struct Position;

struct Point{
  double x, y;
  // arithmetic operator overloads
  Vector operator +(const Point& p2) const;
  Vector operator -(const Point& p2) const;
  
  constexpr Point(): x(0.0), y(0.0){}
  Point(Position position); //not constexpr because position is incomplete at this stage
  constexpr Point(double x, double y): x(x), y(y){}
};

struct Position{
  double x, y, angle;
  constexpr Position(): x(0.0), y(0.0), angle(0.0){}
  constexpr Position(Point point): x(point.x), y(point.y), angle(0.0){}
  constexpr Position(Point point, double angle): x(point.x), y(point.y), angle(angle){}
  constexpr Position(double x, double y, double angle): x(x), y(y), angle(angle){}
};

enum class vector_types {CARTESIAN, POLAR};

class Vector{
    double x, y, magnitude, angle;

  public:
    // constructors
    Vector(const double param_1, const double param_2, vector_types type = vector_types::CARTESIAN);
    Vector(const Point point);

    // configures vector as cartesian vector, with x and y coordinates
    constexpr void set_cartesian(const double x, const double y){
      this->x = x;
      this->y = y;
      magnitude = sqrt(pow(x, 2) + pow(y, 2));
      angle = atan2(y, x);
    }

    constexpr void set_cartesian(const Point point){
      set_cartesian(point.x, point.y);
    }

    // configures vector as polar vector, with an angle and magnitude
    constexpr void set_polar(const double magnitude, const double angle){
      this->magnitude = magnitude;
      this->angle = angle;
      x = magnitude * cos(angle);
      y = magnitude * sin(angle);
    }

    constexpr void rotate(const double rotation_angle){
      set_polar(magnitude, angle + rotation_angle);
    }

    // getters
    double get_x() const;
    double get_y() const;
    double get_magnitude() const;
    double get_angle() const;

    // arithmetic operator overloads
    Vector operator +(const Vector& p2) const;
    Vector operator -(const Vector& p2) const;
};
