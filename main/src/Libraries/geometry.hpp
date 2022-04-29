#pragma once
#include "main.h"

struct Point;
struct Position;
struct Vector;

struct Point{
  double x, y;
  
  //Constructors
    constexpr Point();
    constexpr explicit Point(Vector vector);
    constexpr Point(Position position);
    constexpr Point(double x, double y);

  //Methods
    constexpr Point& invert();
  
  //Operators
    friend constexpr Point operator* (const Point&, double);
    friend constexpr Point operator* (double, const Point&);
    friend constexpr Point operator/ (const Point&, double);
    friend constexpr Point operator/ (double, const Point&);
    constexpr bool operator== (const Point& p2) const;
    constexpr bool operator!= (const Point& p2) const;
    constexpr Point operator+ (const Point& p2) const;
    constexpr Point operator- (const Point& p2) const;
    constexpr Position operator+ (const Position& p2) const;
    constexpr Position operator- (const Position& p2) const;
    constexpr Point& operator*= (double scalar);
    constexpr Point& operator/= (double scalar);
};

struct Position{
  double x, y, angle;

  //Constructors
    constexpr Position();
    constexpr explicit Position(Vector vector);
    constexpr Position(Point point);
    constexpr Position(Point point, double angle);
    constexpr Position(double x, double y);
    constexpr Position(double x, double y, double angle);

    constexpr Position& invert();

  //Operators
    friend constexpr Position operator* (const Position&, double);
    friend constexpr Position operator* (double, const Position&);
    friend constexpr Position operator/ (const Position&, double);
    friend constexpr Position operator/ (double, const Position&);
    constexpr bool operator== (const Position& p2) const;
    constexpr bool operator!= (const Position& p2) const;
    constexpr Position operator+ (const Position& p2) const;
    constexpr Position operator- (const Position& p2) const;
    constexpr Position operator+ (const Point& p2) const;
    constexpr Position operator- (const Point& p2) const;
    constexpr Position& operator*= (double scalar);
    constexpr Position& operator/= (double scalar);
};

struct Vector{
  private:
    double x, y, magnitude, angle;

  public:
    //Constructors
      constexpr Vector();
      constexpr explicit Vector(Point p1);
      constexpr explicit Vector(Point p1, Point p2); //Can't use default argument with previous because that makes it -p2

    //Methods
      constexpr Vector& set_cartesian(Point point);
      constexpr Vector& set_cartesian(Point p2, Point p1);
      constexpr Vector& set_cartesian(double x, double y);
      constexpr Vector& set_polar(double magnitude, double angle);
      constexpr Vector& invert();
      constexpr Vector& rotate(double angle);
      static constexpr Vector make_cartesian(Point point);
      static constexpr Vector make_cartesian(Point p2, Point p1);
      static constexpr Vector make_cartesian(double x, double y);
      static constexpr Vector make_polar(double magnitude, double angle);

    //Operators
      friend constexpr Vector operator* (double, const Vector&);
      friend constexpr Vector operator/ (double, const Vector&);
      constexpr Vector operator* (double) const;
      constexpr Vector operator/ (double) const;
      constexpr bool operator== (const Vector& p2) const;
      constexpr bool operator!= (const Vector& p2) const;
      constexpr Vector operator+ (const Vector& p2) const;
      constexpr Vector operator- (const Vector& p2) const;
      constexpr Vector& operator*= (double scalar);
      constexpr Vector& operator/= (double scalar);

    //Getters
      constexpr double get_x() const;
      constexpr double get_y() const;
      constexpr double get_magnitude() const;
      constexpr double get_angle() const;
};




//Constructors
  //Vector
    constexpr Vector::Vector(): x(0.0), y(0.0), magnitude(0.0), angle(0.0) {};

    constexpr Vector::Vector(Point p1):
    x(0.0), y(0.0), magnitude(0.0), angle(0.0){ //Initializing to 0, becuase constexpr needs to see the initialization
      set_cartesian(p1);
    }

    constexpr Vector::Vector(Point p1, Point p2):
    x(0.0), y(0.0), magnitude(0.0), angle(0.0){ //Initializing to 0, becuase constexpr needs to see the initialization
      set_cartesian(p2-p1);
    }

  //Point
    constexpr Point::Point(): x(0.0), y(0.0){}
    constexpr Point::Point(Vector vector): x(vector.get_x()), y(vector.get_y()){}
    constexpr Point::Point(Position position): x(position.x), y(position.y){}
    constexpr Point::Point(double x, double y): x(x), y(y){}

  //Position
    constexpr Position::Position(): x(0.0), y(0.0), angle(0.0){}
    constexpr Position::Position(Vector vector): x(vector.get_x()), y(vector.get_y()), angle(0.0){}
    constexpr Position::Position(Point point): x(point.x), y(point.y), angle(0.0){}
    constexpr Position::Position(Point point, double angle): x(point.x), y(point.y), angle(angle){}
    constexpr Position::Position(double x, double y): x(x), y(y), angle(0.0){}
    constexpr Position::Position(double x, double y, double angle): x(x), y(y), angle(angle){}


//Methods
  //Vector
    constexpr Vector& Vector::set_cartesian(Point point){
      return set_cartesian(point.x, point.y);
    }

    constexpr Vector& Vector::set_cartesian(Point p2, Point p1){
      return set_cartesian(p2-p1);
    }

    constexpr Vector& Vector::set_cartesian(double x, double y){
      this->x = x;
      this->y = y;
      magnitude = sqrt(pow(x, 2) + pow(y, 2));
      angle = atan2(y, x);
      return *this;
    }

    constexpr Vector& Vector::set_polar(double magnitude, double angle){
      this->magnitude = magnitude;
      this->angle = angle;
      x = magnitude * cos(angle);
      y = magnitude * sin(angle);
      return *this;
    }

    constexpr Vector& Vector::invert(){
      return set_cartesian(y, x);
    }

    constexpr Vector& Vector::rotate(double angle){
      return set_polar(magnitude, this->angle + angle);
    }

    constexpr Vector Vector::make_cartesian(Point point){
      return make_cartesian(point.x, point.y);
    }

    constexpr Vector Vector::make_cartesian(Point p1, Point p2){
      return make_cartesian(p2-p1);
    }

    constexpr Vector Vector::make_cartesian(double x, double y){
      Vector vector;
      vector.set_cartesian(x, y);
      return vector;
    }

    constexpr Vector Vector::make_polar(double magnitude, double angle){
      Vector vector;
      vector.set_polar(magnitude, angle);
      return vector;
    }

    constexpr double Vector::get_x() const{
      return x;
    }
    constexpr double Vector::get_y() const{
      return y;
    }
    constexpr double Vector::get_magnitude() const{
      return magnitude;
    }
    constexpr double Vector::get_angle() const{
      return angle;
    }

  //Point
    constexpr Point& Point::invert(){
      double x = this->x, y = this->y;
      this->x = y, this->y = x;
      return *this;
    }

  //Position
    constexpr Position& Position::invert(){
      double x = this->x, y = this->y;
      this->x = y, this->y = x;
      angle = M_PI_2 - angle;
      return *this;
    }

//Operators
  //Vector
    constexpr Vector Vector::operator* (double scalar) const{
      return make_polar(magnitude * scalar, angle);
    }
    constexpr Vector operator* (double scalar, const Vector& vector){
      return vector*scalar;
    }
    constexpr Vector Vector::operator/ (double scalar) const{
      return *this * (1 / scalar);
    }
    constexpr Vector operator/ (double scalar, const Vector& vector){
      return Vector::make_polar(vector.magnitude / scalar, vector.angle);
    }
    constexpr bool Vector::operator== (const Vector& p2) const{
      return (x == p2.x) && (y == p2.y) && (magnitude == p2.magnitude) && (angle == p2.angle);
    }
    constexpr bool Vector::operator!= (const Vector& p2) const{
      return !(*this == p2);
    }
    constexpr Vector Vector::operator+ (const Vector& p2) const{
      return make_polar(x + p2.x, y + p2.y);
    }
    constexpr Vector Vector::operator- (const Vector& p2) const{
      return make_polar(x - p2.x, y - p2.y);
    }
    constexpr Vector& Vector::operator*= (double scalar){
      return set_polar(magnitude * scalar, angle);
    }
    constexpr Vector& Vector::operator/= (double scalar){
      return set_polar(magnitude / scalar, angle);
    }

  //Point
    constexpr bool Point::operator== (const Point& p2) const{
      return (x == p2.x) && (y == p2.y);
    }
    constexpr bool Point::operator!= (const Point& p2) const{
      return !(*this == p2);
    }
    constexpr Point Point::operator+ (const Point& p2) const{
      return *this - Position(p2);
    }
    constexpr Point Point::operator- (const Point& p2) const{
      return *this - Position(p2);
    }
    constexpr Point& Point::operator*= (double scalar){
      return *this = *this * scalar;
    }
    constexpr Point& Point::operator/= (double scalar){
      return *this = *this / scalar;
    }
    constexpr Point operator* (const Point& point, double scalar){
      return {point.x*scalar, point.y*scalar};
    }
    constexpr Point operator* (double scalar, const Point& point){
      return point*scalar;
    }
    constexpr Point operator/ (const Point& point, double scalar){
      return point*(1 / scalar);
    }
    constexpr Point operator/ (double scalar, const Point& point){
      return {scalar / point.x, scalar / point.y};
    }
    constexpr Position Point::operator+ (const Position& p2) const{
      return {x + p2.x, y + p2.y, p2.angle};
    }
    constexpr Position Point::operator- (const Position& p2) const{
      return {x - p2.x, y - p2.y, p2.angle};
    }

  //Position
    constexpr Position operator* (const Position& position, double scalar){
      return {position.x*scalar, position.y*scalar, position.angle*scalar};
    }
    constexpr Position operator* (double scalar, const Position& position){
      return position*scalar;
    }
    constexpr Position operator/ (const Position& position, double scalar){
      return position * (1 / scalar);
    }
    constexpr Position operator/ (double scalar, const Position& position){
      return {position.x / scalar, position.y / scalar, position.angle / scalar};
    }
    constexpr bool Position::operator== (const Position& p2) const{
      return (x == p2.x) && (y == p2.y) && (angle == p2.angle);
    }
    constexpr bool Position::operator!= (const Position& p2) const{
      return !(*this == p2);
    }
    constexpr Position Position::operator+ (const Position& p2) const{
      return {x + p2.x, y + p2.y, angle + p2.angle};
    }
    constexpr Position Position::operator- (const Position& p2) const{
      return {x - p2.x, y - p2.y, angle - p2.angle};
    }
    constexpr Position Position::operator+ (const Point& p2) const{
      return *this + Position(p2);
    }
    constexpr Position Position::operator- (const Point& p2) const{
      return *this + Position(p2);
    }
    constexpr Position& Position::operator*= (double scalar){
      return *this = *this * scalar;
    }
    constexpr Position& Position::operator/= (double scalar){
      return *this = *this / scalar;
    }