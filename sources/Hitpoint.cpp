//
// Created by Hagen Hiller on 18/12/17.
//

#include "Hitpoint.hpp"


/**
 * c'tor
 */
Hitpoint::Hitpoint()
        : position(glm::vec3())
        , normal(glm::vec3())
        , t(0.0) {}


/**
 * c'tor
 * @param position
 */
Hitpoint::Hitpoint(const glm::vec3 &position)
        : position(position)
        , normal(glm::vec3())
        , t(0.0) {}


/**
 * c'tor
 * @param position
 * @param normal
 * @param t
 */
Hitpoint::Hitpoint(const glm::vec3 &position, const glm::vec3 &normal, double t)
        : position(position)
        , normal(normal)
        , t(t) {}




