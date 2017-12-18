//
// Created by Hagen Hiller on 18/12/17.
//

#ifndef RAYCAST_SPHERE_H
#define RAYCAST_SPHERE_H

#include <glm/glm.hpp>
#include <iosfwd>
#include <ostream>

class Sphere {

public:

    // c'tor
    Sphere();

    Sphere(float);

    Sphere(float, glm::vec3);

    ~Sphere();

    // getter
    float get_radius() const;

    glm::vec3 get_position() const;

    // setter
    void set_radius(float);

    void set_position(glm::vec3);

    friend std::ostream &operator<<(std::ostream &os, const Sphere &sphere);


private:
    float _radius;
    glm::vec3 _position;

};

#endif //RAYCAST_SPHERE_H
