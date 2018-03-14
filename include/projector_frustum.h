//
// Created by everest on 14.03.18.
//

#ifndef RAYCAST_PROJECTOR_FRUSTUM_H
#define RAYCAST_PROJECTOR_FRUSTUM_H

#include <glm/glm.hpp>

class ProjectorFrustum {

public:

    ProjectorFrustum();
    ProjectorFrustum(float fov_v, float aspect_ratio);

private:

    float _aspect_ratio;
    float    _fov;

};

#endif //RAYCAST_PROJECTOR_FRUSTUM_H
