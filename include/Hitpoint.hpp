//
// Created by Hagen Hiller on 18/12/17.
//

#ifndef RAYCAST_HITPOINT_HPP
#define RAYCAST_HITPOINT_HPP

#include <glm/glm.hpp>
#include <ostream>

class Hitpoint {

public:
    // c'tors
    Hitpoint();
    Hitpoint(const glm::vec3 &position);
    Hitpoint(const glm::vec3 &position, const glm::vec3 &normal, double t);
    friend std::ostream &operator<<(std::ostream &os, const Hitpoint &hitpoint);

    // member
    glm::vec3 position;
    glm::vec3 normal;
    double t;
};


#endif //RAYCAST_HITPOINT_HPP

