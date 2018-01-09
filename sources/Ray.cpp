//
// Created by Hagen Hiller on 18/12/17.
//

#include <ostream>
#include <string>
#include <glm/glm.hpp>

#include "Utility.hpp"
#include "Ray.hpp"

/**
 * default c'tor
 */
Ray::Ray()
        : origin(glm::vec3())
        , direction(glm::vec3()) {}

/**
 * c'tor
 * @param origin
 */
Ray::Ray(const glm::vec3 &origin)
        : origin(origin)
        , direction(glm::vec3()) {}

/**
 * c'tor
 * @param origin
 * @param direction
 */
Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
        : origin(origin)
        , direction(direction) {}


/**
 * copy c'tor
 * @param ray
 */
Ray::Ray(const Ray &ray)
        : origin(ray.origin)
        , direction(ray.direction) {}

/**
 * d'tor
 */
Ray::~Ray() {}


/**
 * reflect ray along given normal
 * @param normal
 * @return
 */
glm::vec3 Ray::reflect(glm::vec3 const &normal) const {
    return this->direction - 2.0f * (glm::dot(this->direction, glm::normalize(normal))) * glm::normalize(normal);
}

/**
 * stream << output operator
 * @param os
 * @param ray
 * @return
 */
std::ostream &operator<<(std::ostream &os, const Ray &ray) {
    std::string origin_s = utility::vecstr(ray.origin);
    std::string direction_s = utility::vecstr(ray.direction);
    os << "origin: " << origin_s << " direction: " << direction_s;
    return os;
}
