//
// Created by everest on 14.03.18.  //


#include "projector_frustum.h"
#include "Utility.hpp"

/**
 * Default c'tor
 */
ProjectorFrustum::ProjectorFrustum()
        : _aspect_ratio(1)
        , _fov(90.0f)
        , _position(glm::vec3())
        , _rotation(glm::vec3())
        , _near(0.1f)
        , _far(1.0f)
{}


/**
 * Custom c'tor
 * @param _aspect_ratio
 * @param _fov
 * @param _near
 * @param _far
 * @param _position
 * @param _rotation
 */
ProjectorFrustum::ProjectorFrustum(float _aspect_ratio, float _fov, float _near, float _far, const glm::vec3 &_position,
                                   const glm::vec3 &_rotation)
        : _aspect_ratio(_aspect_ratio)
        , _fov(_fov)
        , _near(_near)
        , _far(_far)
        , _position(_position)
        , _rotation(_rotation)
{

    // define forward vector
    glm::vec3 eye = _position;
    eye.z = -_near;

    // calculate top point
    glm::quat euler(glm::vec3(glm::radians(_fov/2), 0.0f,0.0f));
    glm::vec3 top_point = euler *eye;

    float width = top_point.y * 2;
    glm::vec3 tl(-(width * _aspect_ratio / 2) , top_point.y, top_point.z);
    glm::vec3 bl(-(width * _aspect_ratio / 2) , -top_point.y, top_point.z);
    glm::vec3 tr(width * _aspect_ratio / 2 , top_point.y, top_point.z);
    glm::vec3 br(width * _aspect_ratio / 2 , -top_point.y, top_point.z);

    std::cout << "tl - " << utility::vecstr(tl) << std::endl;
    std::cout << "bl - " << utility::vecstr(bl) << std::endl;
    std::cout << "tr - " << utility::vecstr(tr) << std::endl;
    std::cout << "br - " << utility::vecstr(br) << std::endl;




}
