#include "geometry.hpp"


Point::Point(const double param_1, const double param_2, coord_types type){
  if (type == coord_types::POLAR)  set_polar(param_1, param_2);
  else  set_vector(param_1, param_2);
}

    // configures point as cartesian vector, with x and y coordinates
void Point::set_vector(const double x, const double y){
  this-> x = x, this-> y = y;
  magnitude = sqrt (pow(x, 2) + pow(y, 2)), direction = atan2(y, x);
}

// configures point as polar vector, with an angle and magnitude
void Point::set_polar(const double magnitude, const double direction){
  this-> magnitude = magnitude, this-> direction = direction;
  x = magnitude * cos(direction), y = magnitude * sin(direction);
}

// getters
double Point::get_x() const{
    return this-> x;
}
double Point::get_y() const{
    return this-> y;
}
double Point::get_magnitude() const{
    return this-> magnitude;
}
double Point::get_direction() const{
    return this-> direction;
}

// operator overloads
Point Point::operator +(const Point& p2) const{
    return Point(x + p2.get_x(), y + p2.get_y(), coord_types::VECTOR);
}

Point Point::operator -(const Point& p2) const{
    return Point(x - p2.get_x(), y - p2.get_y(), coord_types::VECTOR);
}



Line::Line(Point p1, Point p2): p1(p1), p2(p2){
    this-> angle = atan2(p2.get_y() - p1.get_y(), p2.get_x() - p1.get_x());
    this-> length = sqrt(pow(p2.get_x() - p1.get_x(), 2) + pow(p2.get_y() - p1.get_y(), 2));
}

// getters

double Line::get_angle() const{
    return this-> angle;
}

double Line::get_length() const{
    return this-> length;
}
Point Line::get_point(short point) const{
    if (point == 1) return p1;
    else return p2;
}

// setters

void Line::set_angle(double angle){
    this-> angle = angle;
    p2.set_vector(p1.get_x() + length * cos(angle), p1.get_y() + length * sin(angle));
}

void Line::set_length(double length){
    this-> length = length;
    p2.set_vector(p1.get_x() + length * cos(angle), p1.get_y() + length * sin(angle));
}
