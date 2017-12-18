//
// Created by Hagen Hiller on 18/12/17.
//

#ifndef RAYCAST_RAY_H
#define RAYCAST_RAY_H

#include <glm/glm.hpp>

struct Ray {

    Ray();
    Ray(glm::vec3, glm::vec3);
    Ray(Ray const &);

    Ray(const glm::vec3 &origin);

    Ray(const glm::vec3 &origin, const glm::vec3 &direction);

    Ray();

    ~Ray();

    virtual ~Ray();

    // members
    glm::vec3 origin;
    glm::vec3 direction;
};

#endif //RAYCAST_RAY_H
