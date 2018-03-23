//
// Created by everest on 14.03.18.  //


#include <glm/gtc/matrix_transform.hpp>
#include "projector_frustum.h"
#include "Utility.hpp"

ProjectorFrustum::ProjectorFrustum()
        : _aspect_ratio(1)
        , _fov(90.0f)
        , _position(glm::vec3())
        , _rotation(glm::vec3())
        , _near(0.1f)
        , _far(1.0f) {}


ProjectorFrustum::ProjectorFrustum(float _aspect_ratio, float _fov, float _near, float _far)
        : _aspect_ratio(_aspect_ratio)
        , _fov(_fov)
        , _near(_near)
        , _far(_far)
        , _eye(glm::vec3(0.0f, 0.0, 1.0f))
        , _position(glm::vec3())
        , _rotation(glm::vec3()) {

    // create foward vector

    _eye.z = -_near;

    // calculate top point
    glm::quat euler({glm::radians(_fov / 2), 0.0f, 0.0f});
    glm::vec3 near_top_point = euler * _eye;

    float width = near_top_point.y * 2.0f;
    glm::vec3 near_tl(-(width * _aspect_ratio / 2.0f), near_top_point.y, near_top_point.z);
    glm::vec3 near_bl(-(width * _aspect_ratio / 2.0f), -near_top_point.y, near_top_point.z);
    glm::vec3 near_tr(width * _aspect_ratio / 2.0f, near_top_point.y, near_top_point.z);
    glm::vec3 near_br(width * _aspect_ratio / 2.0f, -near_top_point.y, near_top_point.z);

    _near_corners[TL] = near_tl;
    _near_corners[TR] = near_tr;
    _near_corners[BR] = near_br;
    _near_corners[BL] = near_bl;

    std::cout << "Near Clipping Corners" << std::endl;
    for (auto p : _near_corners) {
        std::cout << utility::vecstr(p.second) << std::endl;
    }
    std::cout << std::endl;

    _eye.z = -_far;

    // calculate top point
    glm::quat far_euler({glm::radians(_fov / 2.0f), 0.0f, 0.0f});
    glm::vec3 far_top_point = euler * _eye;


    width = far_top_point.y * 2.0f;
    glm::vec3 far_tl(-(width * _aspect_ratio / 2.0f), far_top_point.y, far_top_point.z);
    glm::vec3 far_bl(-(width * _aspect_ratio / 2.0f), -far_top_point.y, far_top_point.z);
    glm::vec3 far_tr(width * _aspect_ratio / 2.0f, far_top_point.y, far_top_point.z);
    glm::vec3 far_br(width * _aspect_ratio / 2.0f, -far_top_point.y, far_top_point.z);

    _far_corners[TL] = far_tl;
    _far_corners[TR] = far_tr;
    _far_corners[BR] = far_br;
    _far_corners[BL] = far_bl;

    std::cout << "Far Clipping Corners" << std::endl;
    for (auto p : _far_corners) {
        std::cout << utility::vecstr(p.second) << std::endl;
    }
    std::cout << std::endl;
}


void ProjectorFrustum::translateTo(glm::vec3 const &position) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
    _position = glm::vec3(translate * glm::vec4(_position, 1.0f));

    _eye = glm::vec3(translate * glm::vec4(_eye, 1.0f));

    // translate each near corner
    for (auto &corner: _near_corners) {
        corner.second = glm::vec3(translate * glm::vec4(corner.second, 1.0f));
    }

    for (auto &corner: _far_corners) {
        corner.second = glm::vec3(translate * glm::vec4(corner.second, 1.0f));
    }
}


void ProjectorFrustum::rotate(float angle, glm::vec3 const &axis) {

    float rad_angle = glm::radians(angle);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rad_angle, axis);

    _eye = glm::vec3(rotation * glm::vec4(_eye, 1.0f));

    for (auto &corner: _near_corners) {
        corner.second = glm::vec3(rotation * glm::vec4(corner.second, 1.0f));
    }

    for (auto &corner: _far_corners) {
        corner.second = glm::vec3(rotation * glm::vec4(corner.second, 1.0f));
    }
}


std::map<ProjectorFrustum::Corner, glm::vec3> const &ProjectorFrustum::getNearCorners() const {
    return _near_corners;
}


std::map<ProjectorFrustum::Corner, glm::vec3> const &ProjectorFrustum::getFarCorners() const {
    return _far_corners;
}

glm::vec3 const &ProjectorFrustum::getEye() const {
    return _eye;
}
