#include "geometry.hpp"

// point operators
Vector Point::operator +(const Point& p2) const{
  return Vector(x + p2.x, y + p2.y, vector_types::CARTESIAN);
}
Vector Point::operator -(const Point& p2) const{
  return Vector(x - p2.x, y - p2.y, vector_types::CARTESIAN);
}

// Non-Constexpr Point Constructor
Point::Point(Position position): x(position.x), y(position.y){}

// vector methods
Vector::Vector(const double param_1, const double param_2, vector_types type){
  if (type == vector_types::POLAR)  set_polar(param_1, param_2);
  else  set_cartesian(param_1, param_2);
}

Vector::Vector(const Point point){
  set_cartesian(point);
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
