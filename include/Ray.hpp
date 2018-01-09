//
// Created by Hagen Hiller on 18/12/17.
//

#ifndef RAYCAST_RAY_H
#define RAYCAST_RAY_H

#include <glm/glm.hpp>
#include <ostream>

struct Ray {

    Ray();
    Ray(const glm::vec3 &origin);
    Ray(const glm::vec3 &origin, const glm::vec3 &direction);
    Ray(const Ray& ray);
    ~Ray();


    glm::vec3 reflect(glm::vec3 const& normal) const;
    friend std::ostream &operator<<(std::ostream &os, const Ray &ray);

    // members
    glm::vec3 origin;
    glm::vec3 direction;
};

#endif //RAYCAST_RAY_H
