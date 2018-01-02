//
// Created by Hagen Hiller on 20/12/17.
//

#ifndef RAYCAST_UTILITY_HPP
#define RAYCAST_UTILITY_HPP


#include <string>
#include <glm/vec3.hpp>

namespace utility {

    std::string vecstr(glm::vec3 vec);

    double d2r(float deg);

    double r2d(float rad);

    float angle(glm::vec3 a, glm::vec3 b);
}


#endif //RAYCAST_UTILITY_HPP
