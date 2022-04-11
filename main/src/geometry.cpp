#include "geometry.hpp"

// point operators
Vector Point::operator +(const Point& p2) const{
  return Vector(x + p2.x, y + p2.y, vector_types::CARTESIAN);
}
Vector Point::operator -(const Point& p2) const{
  return Vector(x - p2.x, y - p2.y, vector_types::CARTESIAN);
}

// point constructors
Point::Point(): x(0.0), y(0.0){}
Point::Point(Position position): x(position.x), y(position.y){}
Point::Point(double x, double y): x(x), y(y){}

// position constructors
Position::Position(): x(0.0), y(0.0), angle(0.0){}
Position::Position(Point point): x(point.x), y(point.y), angle(0.0){}
Position::Position(Point point, double angle): x(point.x), y(point.y), angle(angle){}
Position::Position(double x, double y, double angle): x(x), y(y), angle(angle){}

// vector methods
Vector::Vector(const double param_1, const double param_2, vector_types type){
  if (type == vector_types::POLAR)  set_polar(param_1, param_2);
  else  set_cartesian(param_1, param_2);
}

Vector::Vector(const Point point){
  set_cartesian(point);
}

// configures point as cartesian vector, with x and y coordinates
void Vector::set_cartesian(const double x, const double y){
  this-> x = x, this-> y = y;
  magnitude = sqrt (pow(x, 2) + pow(y, 2)), angle = atan2(y, x);
}
// cartesian vector, taking in a point instead
void Vector::set_cartesian(const Point point){
  this-> x = point.x, this-> y = point.y;
  magnitude = sqrt (pow(x, 2) + pow(y, 2)), angle = atan2(y, x);
}

// configures point as polar vector, with an angle and magnitude
void Vector::set_polar(const double magnitude, const double angle){
  this-> magnitude = magnitude, this-> angle = angle;
  x = magnitude * cos(angle), y = magnitude * sin(angle);
}

// configures point as polar vector, with an angle and magnitude
void Vector::rotate(const double rotation_angle){
  set_polar(magnitude, angle + rotation_angle);
}

// getters
double Vector::get_x() const{
    return this-> x;
}
double Vector::get_y() const{
    return this-> y;
}
double Vector::get_magnitude() const{
    return this-> magnitude;
}
double Vector::get_angle() const{
    return this-> angle;
}

// Vector operator overloads
Vector Vector::operator +(const Vector& p2) const{
    return Vector(x + p2.get_x(), y + p2.get_y(), vector_types::CARTESIAN);
}

Vector Vector::operator -(const Vector& p2) const{
    return Vector(x - p2.get_x(), y - p2.get_y(), vector_types::CARTESIAN);
}
