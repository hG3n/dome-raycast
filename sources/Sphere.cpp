//
// Created by Hagen Hiller on 18/12/17.
//
#include <glm/glm.hpp>
#include "Sphere.hpp"

#define epsilon 0.000001f

// -----------------------------------------------------------------------------------------
// C'TORs

/**
 * default c'tor
 */
Sphere::Sphere()
        : _radius(1.0f)
        , _center(glm::vec3()) {}


/**
 * c'tor
 * @param radius
 */
Sphere::Sphere(float radius)
        : _radius(radius)
        , _center(glm::vec3()) {}


/**
 * c'tor
 * @param radius
 * @param position
 */
Sphere::Sphere(float radius, glm::vec3 position)
        : _radius(radius)
        , _center(position) {}


/**
 * d'tor
 */
Sphere::~Sphere() = default;

// -----------------------------------------------------------------------------------------
// METHODS

/**
 * calculate intersection
 * @param r
 * @param tmin
 * @param hp
 * @return
 */
bool Sphere::intersect(Ray const &r, double &tmin, Hitpoint *hp) {

    double t;
    glm::vec3 temp = r.origin - _center;
    double a = glm::dot(r.direction, r.direction);
    double b = 2.0 * dot(temp, r.direction);
    double c = dot(temp, temp) - _radius * _radius;
    double disc = b * b - 4.0 * a * c;

    if (disc < 0.0)
        return (false);
    else {
        double e = std::sqrt(disc);
        double denom = 2.0 * a;
        t = (-b - e) / denom;   // smaller root

        if (t > epsilon) {
            tmin = t;
            hp->normal = glm::normalize((temp + t * r.direction) / _radius);
            hp->position = r.origin + t * r.direction;
            return true;
        }
        t = (-b + e) / denom; // larger root

        if (t > epsilon) {
            tmin = t;
            hp->normal = glm::normalize((temp + t * r.direction) / _radius);
            hp->position = r.origin + t * r.direction;
            return true;
        }
    }
    return false;
}


// -----------------------------------------------------------------------------------------
// GETTER

/**
 * get radius
 * @return
 */
float Sphere::get_radius() const {
    return _radius;
}

/**
 * get position
 * @return
 */
glm::vec3 Sphere::get_position() const {
    return _center;
}

// -----------------------------------------------------------------------------------------
// SETTER

/**
 * set position
 * @param new_position
 */
void Sphere::set_position(glm::vec3 new_position) {
    _center = new_position;
}


/**
 * set radius
 * @param new_radius
 */
void Sphere::set_radius(float new_radius) {
    _radius = new_radius;
}

// -----------------------------------------------------------------------------------------
// OPERATORS

/**
 * overloaded out stream operator
 * @param os
 * @param sphere
 * @return
 */
std::ostream &operator<<(std::ostream &os, const Sphere &sphere) {
    os << "<Sphere | radius: " << sphere._radius
       << " | center: ["
       << sphere._center.x << ","
       << sphere._center.y << ","
       << sphere._center.z << "]"
       << ">";
    return os;
}
