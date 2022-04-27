#pragma once
#include "main.h"

//No .cpp since everything here is constexpr

struct Point;
struct Position;
enum class vector_types {CARTESIAN, POLAR};

struct Vector{
  private:
    double x, y, magnitude, angle;

  public:
    //Constructors
      constexpr Vector(): x(0.0), y(0.0), magnitude(0.0), angle(0.0) {};
      constexpr explicit Vector(Point p1);
      constexpr Vector(Point p1, Point p2); //Can't use default argument with previous because Point is incomplete
      constexpr Vector(const double param_1, const double param_2, vector_types type = vector_types::CARTESIAN):
      x(0.0), y(0.0), magnitude(0.0), angle(0.0){ //Initializing to 0, becuase constexpr needs to see the initialization
        if (type == vector_types::POLAR) set_polar(param_1, param_2);
        else set_cartesian(param_1, param_2);
      }

    //Methods
      constexpr void set_cartesian(const Point point);

      constexpr void set_cartesian(const double x, const double y){
        this->x = x;
        this->y = y;
        magnitude = sqrt(pow(x, 2) + pow(y, 2));
        angle = atan2(y, x);
      }

      constexpr void set_polar(const double magnitude, const double angle){
        this->magnitude = magnitude;
        this->angle = angle;
        x = magnitude * cos(angle);
        y = magnitude * sin(angle);
      }

      constexpr void rotate(const double rotation_angle){
        set_polar(magnitude, angle + rotation_angle);
      }

    //Arithmetic Operators
      friend constexpr Vector operator* (const Vector&, double);
      friend constexpr Vector operator* (double, const Vector&);
      friend constexpr Vector operator/ (const Vector&, double);
      friend constexpr Vector operator/ (double, const Vector&);
      constexpr Vector operator+ (const Vector& p2) const{
        return {x + p2.x, y + p2.y};
      }
      constexpr Vector operator- (const Vector& p2) const{
        return {x - p2.x, y - p2.y};
      }
      constexpr Vector operator*= (double scalar){
        *this = *this * scalar;
        return *this;
      }
      constexpr Vector operator/= (double scalar){
        *this = *this / scalar;
        return *this;
      }

    //Getters
      constexpr double get_x() const{
        return x;
      }
      constexpr double get_y() const{
        return y;
      }
      constexpr double get_magnitude() const{
        return magnitude;
      }
      constexpr double get_angle() const{
        return angle;
      }
};

//Vector Operators
  constexpr Vector operator* (const Vector& vector, double scalar){
    return Vector(vector.get_magnitude() * scalar, vector.get_angle(), vector_types::POLAR);
  }
  constexpr Vector operator* (double scalar, const Vector& vector){
    return vector*scalar;
  }
  constexpr Vector operator/ (const Vector& vector, double scalar){
    return vector*(1/scalar);
  }
  constexpr Vector operator/ (double scalar, const Vector& vector){
    return Vector(vector.get_magnitude() / scalar, vector.get_angle(), vector_types::POLAR);
  }

struct Point{
  double x, y;
  
  //Constructors
    constexpr Point(): x(0.0), y(0.0){}
    constexpr explicit Point(Vector vector): x(vector.get_x()), y(vector.get_y()){}
    constexpr Point(Position position);
    constexpr Point(double x, double y): x(x), y(y){}
  
  //Arithmetic Operators
    friend constexpr Point operator* (const Point&, double);
    friend constexpr Point operator* (double, const Point&);
    friend constexpr Point operator/ (const Point&, double);
    friend constexpr Point operator/ (double, const Point&);
    constexpr Point operator+ (const Point& p2) const{
      return {x + p2.x, y + p2.y};
    }
    constexpr Point operator- (const Point& p2) const{
      return {x - p2.x, y - p2.y};
    }
    constexpr Point operator*= (double scalar){
      *this = *this * scalar;
      return *this;
    }
    constexpr Point operator/= (double scalar){
      *this = *this / scalar;
      return *this;
    }
    constexpr Position operator+ (const Position& p2) const;
    constexpr Position operator- (const Position& p2) const;
};

// Point Operators
  constexpr Point operator* (const Point& point, double scalar){
    return {point.x*scalar, point.y*scalar};
  }
  constexpr Point operator* (double scalar, const Point& point){
    return point*scalar;
  }
  constexpr Point operator/ (const Point& point, double scalar){
    return point*(1/scalar);
  }
  constexpr Point operator/ (double scalar, const Point& point){
    return {scalar/point.x, scalar/point.y};
  }


struct Position{
  double x, y, angle;

  //Constructors
    constexpr Position(): x(0.0), y(0.0), angle(0.0){}
    constexpr explicit Position(Vector vector): x(vector.get_x()), y(vector.get_y()), angle(0.0){}
    constexpr Position(Point point): x(point.x), y(point.y), angle(0.0){}
    constexpr Position(Point point, double angle): x(point.x), y(point.y), angle(angle){}
    constexpr Position(double x, double y): x(x), y(y), angle(0.0){}
    constexpr Position(double x, double y, double angle): x(x), y(y), angle(angle){}

  //Arithmetic Operators
    friend constexpr Position operator* (const Position&, double);
    friend constexpr Position operator* (double, const Position&);
    friend constexpr Position operator/ (const Position&, double);
    friend constexpr Position operator/ (double, const Position&);
    constexpr Position operator+ (const Position& p2) const{
      return {x + p2.x, y + p2.y, angle + p2.angle};
    }
    constexpr Position operator- (const Position& p2) const{
      return {x - p2.x, y - p2.y, angle - p2.angle};
    }
    constexpr Position operator+ (const Point& p2) const{
      return {x + p2.x, y + p2.y, angle};
    }
    constexpr Position operator- (const Point& p2) const{
      return {x - p2.x, y - p2.y, angle};
    }
    constexpr Position operator*= (double scalar){
      *this = *this * scalar;
      return *this;
    }
    constexpr Position operator/= (double scalar){
      *this = *this / scalar;
      return *this;
    }
};

//Position Operators
  constexpr Position operator* (const Position& position, double scalar){
    return {position.x*scalar, position.y*scalar, position.angle};
  }
  constexpr Position operator* (double scalar, const Position& position){
    return position*scalar;
  }
  constexpr Position operator/ (const Position& position, double scalar){
    return position*(1/scalar);
  }
  constexpr Position operator/ (double scalar, const Position& position){
    return {position.x/scalar, position.y/scalar, position.angle};
  }


//These have to be defined outside since not all info is available at time of declaration
constexpr Point::Point(Position position): x(position.x), y(position.y){}
constexpr Vector::Vector(Point p1):
x(0.0), y(0.0), magnitude(0.0), angle(0.0){ //Initializing to 0, becuase constexpr needs to see the initialization
  set_cartesian(p1);
}

constexpr Vector::Vector(Point p1, Point p2):
x(0.0), y(0.0), magnitude(0.0), angle(0.0){ //Initializing to 0, becuase constexpr needs to see the initialization
  set_cartesian(p2-p1);
}

constexpr void Vector::set_cartesian(const Point point){
  set_cartesian(point.x, point.y);
}

constexpr Position Point::operator+ (const Position& p2) const{
  return {x + p2.x, y + p2.y, p2.angle};
}
constexpr Position Point::operator- (const Position& p2) const{
  return {x - p2.x, y - p2.y, p2.angle};
}