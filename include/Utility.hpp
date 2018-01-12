//
// Created by Hagen Hiller on 20/12/17.
//

#ifndef RAYCAST_UTILITY_HPP
#define RAYCAST_UTILITY_HPP


#include <string>
#include <glm/vec3.hpp>
#include <vector>

namespace utility {

    std::string vecstr(glm::vec3 vec);

    double d2r(float deg);

    double r2d(float rad);

    float angle(glm::vec3 a, glm::vec3 b);

    float mapToRange(float value, float in_min, float in_max, float out_min, float out_max);

    glm::vec3 findMinValues(std::vector<glm::vec3> vector);

    glm::vec3 findMaxValues(std::vector<glm::vec3> vector);
}

#endif //RAYCAST_UTILITY_HPP
