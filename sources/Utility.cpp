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
    return acos(glm::dot(a, b) / (glm::length(a) * glm::length(b)));
}


/**
 * maps a value to a certain range
 * @param value
 * @param in_min
 * @param in_max
 * @param out_min
 * @param out_max
 * @return
 */
float utility::mapToRange(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * finds minimum values of each axis of a list of vectors
 * @param vector
 * @return
 */
glm::vec3 utility::findMinValues(std::vector<glm::vec3> vector) {

    float smallest_x = std::numeric_limits<float>::max();
    float smallest_y = smallest_x;
    float smallest_z = smallest_x;

    for (auto point: vector) {
        if (point.x < smallest_x) {
            smallest_x = point.x;
        }

        if (point.y < smallest_y) {
            smallest_y = point.y;
        }

        if (point.z < smallest_z) {
            smallest_z = point.z;
        }
    }

    return glm::vec3(smallest_x, smallest_y, smallest_z);

}


/**
 * finds maximum values of each axis of a list of vectors
 * @param vector
 * @return
 */
glm::vec3 utility::findMaxValues(std::vector<glm::vec3> vector) {

    float smallest_x = std::numeric_limits<float>::min();
    float smallest_y = smallest_x;
    float smallest_z = smallest_x;

    for (auto point: vector) {
        if (point.x > smallest_x) {
            smallest_x = point.x;
        }

        if (point.y > smallest_y) {
            smallest_y = point.y;
        }

        if (point.z > smallest_z) {
            smallest_z = point.z;
        }
    }

    return glm::vec3(smallest_x, smallest_y, smallest_z);

}
