//
// Created by Hagen Hiller on 18/12/17.
//

#ifndef RAYCAST_SPHERE_H
#define RAYCAST_SPHERE_H

#include <glm/glm.hpp>
#include <iosfwd>
#include <ostream>
#include "Ray.hpp"
#include "Hitpoint.hpp"

class Sphere {

public:

    // c'tor
    Sphere();
    Sphere(float);
    Sphere(float, glm::vec3);
    ~Sphere();

    // methods
    bool intersect(Ray const &r, std::pair<Hitpoint, Hitpoint> *hp_pair);

    // getter
    float get_radius() const;
    glm::vec3 get_position() const;

    // setter
    void set_radius(float);
    void set_position(glm::vec3);

    // ostream
    friend std::ostream &operator<<(std::ostream &os, const Sphere &sphere);


private:
    float _radius;
    glm::vec3 _center;

};

#endif //RAYCAST_SPHERE_H
