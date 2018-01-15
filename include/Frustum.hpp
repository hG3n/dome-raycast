//
// Created by Hagen Hiller on 09/01/18.
//

#ifndef RAYCAST_FRUSTUM_HPP
#define RAYCAST_FRUSTUM_HPP

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Frustum {

    Frustum();
    Frustum(glm::mat4 const &projection_mat, glm::vec3 const &world_position, bool);
    ~Frustum();

    glm::mat4 _projection_mat;

    glm::vec3 _position;

    // frustum corners
    std::vector<glm::vec3> _near_clipping_corners;
    /* frustum near clipping plane indices
     * 0 ---- 1
     * |      |
     * 3 ---- 2
     */

    std::vector<glm::vec3> _far_clipping_corners;
};


#endif //RAYCAST_FRUSTUM_HPP
