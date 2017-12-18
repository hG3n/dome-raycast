//
// Created by Hagen Hiller on 18/12/17.
//

#include <Ray.h>

Ray::Ray()
        : origin(glm::vec3()), direction(glm::vec3()) {}

/**
 * c'tor
 * @param origin
 */
Ray::Ray(const glm::vec3 &origin)
        : origin(origin) {}

/**
 * c'tor
 * @param origin
 * @param direction
 */
Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
        : origin(origin), direction(direction) {}

/**
 * d'tor
 */
Ray::~Ray() {}


