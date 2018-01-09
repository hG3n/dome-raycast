//
// Created by Hagen Hiller on 18/12/17.
//

#include "Utility.hpp"
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


/**
 * stream output operator
 * @param os
 * @param hitpoint
 * @return outstream
 */
std::ostream &operator<<(std::ostream &os, const Hitpoint &hitpoint) {
    std::string pos_string = utility::vecstr(hitpoint.position);
    std::string norm_string = utility::vecstr(hitpoint.normal);
    os << "position: " << pos_string << " normal: " << norm_string << " t: " << hitpoint.t;
    return os;
}




