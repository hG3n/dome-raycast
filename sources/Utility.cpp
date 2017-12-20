//
// Created by Hagen Hiller on 20/12/17.
//

#include <cmath>
#include <glm/geometric.hpp>
#include "Utility.hpp"

/**
 * returns printable string of a glm::vec3
 * @param vec
 * @return printable glm::vec3 string
 */
std::string utility::vecstr(glm::vec3 vec) {
    return "[" + std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z) + "]";
}

/**
 * deg to rad
 * @param deg
 * @return
 */
double utility::d2r(float deg) {
    return deg * M_PI / 180.0f;
}

/**
 * rad to deg
 * @param rad
 * @return
 */
double utility::r2d(float rad) {
    return rad * 180.0f / M_PI;
}


/**
 * calc the angle between two vectors
 * @param a
 * @param b
 * @return
 */
float utility::angle(glm::vec3 a, glm::vec3 b) {
    return acos(glm::dot(a,b) / (glm::length(a) * glm::length(b)));
}



