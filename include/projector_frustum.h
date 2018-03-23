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
    enum ClippingPlane {
        NEAR, FAR
    };
    enum Corner {
        TL, TR, BL, BR
    };

    /**
     * Default c'tor
     */
    ProjectorFrustum();

    /**
     * Custom c'tor
     * creates new frustum in
     * @param _aspect_ratio
     * @param _fov
     * @param _near
     * @param _far
     */
    ProjectorFrustum(float _aspect_ratio, float _fov, float _near, float _far);

    /**
     * Translate to current position
     * @param position
     */
    void translateTo(glm::vec3 const &position);

    /**
     * Rotate by given degree angle around specified axis
     * @param angle
     * @param axis
     */
    void rotate(float angle, glm::vec3 const &axis);

    /**
     * Returns near clipping plane corners
     * @return
     */
    std::map<Corner, glm::vec3> const& getNearCorners() const;

    /**
     * Returns far clipping plane corners
     * @return
     */
    std::map<Corner, glm::vec3> const& getFarCorners() const;


    /**
     * returns frustum eye ray
     * @return
     */
    glm::vec3 const& getEye() const;

private:

    float _aspect_ratio;
    float _fov;
    float _near;
    float _far;

    glm::vec3 _eye;
    glm::vec3 _position;
    glm::vec3 _rotation;

    std::map<Corner, glm::vec3> _near_corners;
    std::map<Corner, glm::vec3> _far_corners;

};

#endif //RAYCAST_PROJECTOR_FRUSTUM_H
