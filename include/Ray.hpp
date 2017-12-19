//
// Created by Hagen Hiller on 18/12/17.
//

#ifndef RAYCAST_RAY_H
#define RAYCAST_RAY_H

#include <glm/glm.hpp>

struct Ray {

    Ray();
    Ray(const glm::vec3 &origin);
    Ray(const glm::vec3 &origin, const glm::vec3 &direction);
    Ray(const Ray& ray);
    ~Ray();

    // members
    glm::vec3 origin;
    glm::vec3 direction;
};

#endif //RAYCAST_RAY_H
