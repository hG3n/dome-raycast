//
// Created by Hagen Hiller on 09/01/18.
//


#include "Frustum.hpp"
#include "Utility.hpp"

/**
 * TODO: make dome projector build the frustum
 */


/**
 * default c'tor
 */
Frustum::Frustum() {}


/**
 * Calculates the Frustum corners according to the given projection
 * @param _projection_mat
 */
Frustum::Frustum(glm::mat4 const &_projection_mat, glm::vec3 const& world_position, bool verbose = false)
        : _projection_mat(_projection_mat) {

    std::vector<glm::vec4> corners(8);

    // near
    corners[0] = glm::vec4(-1, 1, 1, 1);
    corners[1] = glm::vec4(1, 1, 1, 1);
    corners[2] = glm::vec4(1, -1, 1, 1);
    corners[3] = glm::vec4(-1, -1, 1, 1);

    // far
    corners[4] = glm::vec4(-1, 1, -1, 1);
    corners[5] = glm::vec4(1, 1, -1, 1);
    corners[6] = glm::vec4(1, -1, -1, 1);
    corners[7] = glm::vec4(-1, -1, -1, 1);

    glm::mat4 inv_proj = glm::inverse(_projection_mat);

    for (unsigned int i = 0; i < corners.size(); ++i) {
        corners[i] = corners[i] * inv_proj;
        corners[i] /= corners[i].w;

        // translate to world posiiotn
        glm::vec3 corner_world = glm::vec3(corners[i])  + world_position;

        if(i < 4) {
            _near_clipping_corners.emplace_back(corner_world);
        } else {
            _far_clipping_corners.emplace_back(corner_world);
        }
    }

    if(verbose) {
        std::cout << "Frustum corners" << std::endl;
        std::cout << "===============" << std::endl;
        std::cout << "  Near:" << std::endl;
        for (auto I : _near_clipping_corners) {
            std::cout << "     " << utility::vecstr(I) << std::endl;
        }

        std::cout << "  Far:" << std::endl;
        for (auto I : _far_clipping_corners) {
            std::cout << "     " << utility::vecstr(I) << std::endl;
        }
        std::cout << std::endl;
    }

}

/**
 * Destructor
 */
Frustum::~Frustum() {}
