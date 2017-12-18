//
// Created by Hagen Hiller on 18/12/17.
//
#include <glm/glm.hpp>
#include "Sphere.h"

// -----------------------------------------------------------------------------------------
// C'TORs

/**
 * default c'tor
 */
Sphere::Sphere()
        : _radius(1.0f)
        , _position(glm::vec3()) {}


/**
 * c'tor
 * @param radius
 */
Sphere::Sphere(float radius)
        : _radius(radius)
        , _position(glm::vec3()) {}


/**
 * c'tor
 * @param radius
 * @param position
 */
Sphere::Sphere(float radius, glm::vec3 position)
        : _radius(radius)
        , _position(position) {}


/**
 * d'tor
 */
Sphere::~Sphere() {}

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
    return _position;
}

// -----------------------------------------------------------------------------------------
// SETTER

/**
 * set position
 * @param new_position
 */
void Sphere::set_position(glm::vec3 new_position) {
    _position = new_position;
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
       << sphere._position.x << ","
       << sphere._position.y << ","
       << sphere._position.z << "]"
       << ">";
    return os;
}
