#include "geometry.hpp"


Point::Point(const double param_1, const double param_2, coord_types type = coord_types::VECTOR){
        if (type == coord_types::POLAR)  set_polar(param_1, param_2);
        else    set_vector(param_1, param_2);
    }

    // configures point as cartesian vector, with x and y coordinates
    void set_vector(const double x, const double y){
        this-> x = x, this-> y = y;
        magnitude = sqrt (pow(x, 2) + pow(y, 2)), direction = atan2(y, x);
    }

    // configures point as polar vector, with an angle and magnitude
    void set_polar(const double magnitude, const double direction){
        this-> magnitude = magnitude, this-> direction = direction;
        x = magnitude * cos(direction), y = magnitude * sin(direction);
    }

    // getters
    double get_x() const{
        return this-> x;
    }
    double get_y() const{
        return this-> y;
    }
    double get_magnitude() const{
        return this-> magnitude;
    }
    double get_direction() const{
        return this-> direction;
    }

    // operator overloads
    point operator +(const point& p2) const{
        return point(x + p2.get_x(), y + p2.get_y(), coord_types::VECTOR);
    }

    point operator -(const point& p2) const{
        return point(x - p2.get_x(), y - p2.get_y(), coord_types::VECTOR);
    }

};




    line(point p1, point p2): p1(p1), p2(p2){
        this-> angle = atan2(p2.get_y() - p1.get_y(), p2.get_x() - p1.get_x());
        this-> length = sqrt(pow(p2.get_x() - p1.get_x(), 2) + pow(p2.get_y() - p1.get_y(), 2));
    }

    // getters

    double get_angle(){
        return this-> angle;
    }

    double get_length(){
        return this-> length;
    }
    point get_point(short point = 1){
        if (point == 1) return p1;
        else return p2;
    }

    // setters

    void set_angle(double angle){
        this-> angle = angle;
        p2.set_vector(p1.get_x() + length * cos(angle), p1.get_y() + length * sin(angle));
    }

    void set_length(double length){
        this-> length = length;
        p2.set_vector(p1.get_x() + length * cos(angle), p1.get_y() + length * sin(angle));
    }

    void set_x_of_point(double x, short point){
        switch (point){
            case 1:
                p1.set_vector(x, this->p1.get_y());
            case 2:
                p2.set_vector(x, this->p2.get_y());
        }
    }
    void set_y_of_point(double y, short point){
        switch (point){
            case 1:
                p1.set_vector(p1.get_x(), y);
            case 2:
                p2.set_vector(p2.get_x(), y);
        }
    }

};
