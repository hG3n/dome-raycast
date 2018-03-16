//
// Created by everest on 14.03.18.
//

#ifndef RAYCAST_PROJECTOR_FRUSTUM_H
#define RAYCAST_PROJECTOR_FRUSTUM_H

#include <iostream>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class ProjectorFrustum {

public:

    ProjectorFrustum();
    ProjectorFrustum(float _aspect_ratio, float _fov, float _near, float _far, const glm::vec3 &_position,
                     const glm::vec3 &_rotation);


private:

    float _aspect_ratio;
    float _fov;
    float _near;
    float _far;

    glm::vec3 _position;
    glm::vec3 _rotation;

    std::map<std::string, glm::vec3> _near_corners;
    std::map<std::string, glm::vec3> _far_corners;

};

#endif //RAYCAST_PROJECTOR_FRUSTUM_H
