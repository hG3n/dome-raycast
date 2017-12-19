//
// Created by Hagen Hiller on 18/12/17.
//

#include <Ray.hpp>

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
