#include "geometry.hpp"

// Position constructor definition
Position::Position(double x, double y, double angle): x(x), y(y), angle(angle){}
Position::Position(): x(0.0), y(0.0), angle(0.0){}

// vector methods

Vector::Vector(const double param_1, const double param_2, vector_types type){
  if (type == vector_types::POLAR)  set_polar(param_1, param_2);
  else  set_cartesian(param_1, param_2);
}

// configures point as cartesian vector, with x and y coordinates
void Vector::set_cartesian(const double x, const double y){
  this-> x = x, this-> y = y;
  magnitude = sqrt (pow(x, 2) + pow(y, 2)), angle = atan2(y, x);
}

// configures point as polar vector, with an angle and magnitude
void Vector::set_polar(const double magnitude, const double angle){
  this-> magnitude = magnitude, this-> angle = angle;
  x = magnitude * cos(angle), y = magnitude * sin(angle);
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

// operator overloads
Vector Vector::operator +(const Vector& p2) const{
    return Vector(x + p2.get_x(), y + p2.get_y(), vector_types::CARTESIAN);
}

Vector Vector::operator -(const Vector& p2) const{
    return Vector(x - p2.get_x(), y - p2.get_y(), vector_types::CARTESIAN);
}
