//
// Created by Hagen Hiller on 18/12/17.
//
#include <iostream>
#include <glm/glm.hpp>

#include "Sphere.hpp"

#define epsilon 0.000001f

// -----------------------------------------------------------------------------------------
// C'TORs

/**
 * default c'tor
 */
Sphere::Sphere()
        : _radius(1.0f), _center(glm::vec3()) {}


/**
 * c'tor
 * @param radius
 */
Sphere::Sphere(float radius)
        : _radius(radius), _center(glm::vec3()) {}


/**
 * c'tor
 * @param radius
 * @param position
 */
Sphere::Sphere(float radius, glm::vec3 position)
        : _radius(radius), _center(position) {}


/**
 * d'tor
 */
Sphere::~Sphere() = default;

// -----------------------------------------------------------------------------------------
// METHODS

/**
 * Validates, whether the given ray intersects the Sphere and retunts a pair of Hitpoints.
 * @param r
 * @param hp_pair
 * @return bool success
 */
bool Sphere::intersect(Ray const &r, std::pair<Hitpoint, Hitpoint> *hp_pair) {

    bool verbose = false;

    // vector from current ray orign to sphere center
    glm::vec3 L = this->_center - r.origin;

    // distance from ray origin to intersection with the spheres perpendicular vector
    double t_ca = glm::dot(L, r.direction);

    // if t_ca is smaller than 0 the ray might be pointing in the wrong direction
    if (t_ca < 0) {
        if(verbose)
            std::cout << "t_ca < 0" << std::endl;
        return false;
    }

    // calc vector length perpendicular to the ray through the spheres center
    double d = sqrt(glm::dot(L, L) - pow(t_ca, 2));
    if (d < 0 || d > this->_radius) {

        if(verbose) {
            if (d < 0)
                std::cout << "d < 0" << std::endl;

            if (d > this->_radius)
                std::cout << "d > _radius" << std::endl;
        }

        return false;
    }

    // calculate the distance from the first hitpoint to d
    double t_hc = sqrt(pow(this->_radius, 2) - pow(d, 2));

    double t_0 = t_ca - t_hc;
    double t_1 = t_ca + t_hc;

    // help vector
    glm::vec3 temp = r.origin - this->_center;

    // first hitpoint
    glm::vec3 P1 = r.origin + ((float) t_0 * r.direction);
    glm::vec3 N1 = glm::normalize((temp + (float)t_0 * r.direction) / this->_radius);
    hp_pair->first.position = P1;
    hp_pair->first.normal = N1;

    // second hitpoint
    glm::vec3 P2 = r.origin + ((float) t_1 * r.direction);
    glm::vec3 N2 = glm::normalize((temp + (float)t_1 * r.direction) / this->_radius);
    hp_pair->second.position = P2;
    hp_pair->second.normal = N2;

    return true;
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
