#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Sphere.hpp"


int SAMPLE_RINGS = 10;
int SAMPLE_RING_POINTS = 36;

enum Axis {
    X, Y, Z
};

/**
 * calculate frustum corners according to the current projection matrix
 * @param corners
 * @param projection
 */
void calculateFrustumCorners(std::vector<glm::vec3> *corners, glm::mat4 projection) {

    glm::vec4 hcorners[8];

    // near
    hcorners[0] = glm::vec4(-1, 1, 1, 1);
    hcorners[1] = glm::vec4(1, 1, 1, 1);
    hcorners[2] = glm::vec4(1, -1, 1, 1);
    hcorners[3] = glm::vec4(-1, -1, 1, 1);

    // far
    hcorners[4] = glm::vec4(-1, 1, -1, 1);
    hcorners[5] = glm::vec4(1, 1, -1, 1);
    hcorners[6] = glm::vec4(1, -1, -1, 1);
    hcorners[7] = glm::vec4(-1, -1, -1, 1);

    glm::mat4 inv_proj = glm::inverse(projection);

    for (int i = 0; i < hcorners->length(); ++i) {
        hcorners[i] = hcorners[i] * inv_proj;
        hcorners[i] /= hcorners[i].w;

        corners->push_back(glm::vec3(hcorners[i]));
    }
}


/**
 * returns printable string of a glm::vec3
 * @param vec
 * @return printable glm::vec3 string
 */
std::string vecstr(glm::vec3 vec) {
    return "[" + std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z) + "]";
}


/**
 * deg to rad
 * @param deg
 * @return
 */
double d2r(float deg) {
    return deg * M_PI / 180.0f;
}

/**
 * rad to deg
 * @param rad
 * @return
 */
double r2d(float rad) {
    return rad * 180.0f / M_PI;
}





/**
 * generates a radial grid along given axis
 * @param axis
 * @return vector containing grid points
 */
std::vector<glm::vec3> generateRadialGrid(Axis axis, std::vector<glm::vec3> frustum_corners) {

    float step_size = 0;
    switch (axis) {

        case X:
            step_size = ((frustum_corners[0].x - frustum_corners[1].x) / 2) / SAMPLE_RINGS;
            break;
        case Y:
            step_size = ((frustum_corners[0].y - frustum_corners[3].y) / 2) / SAMPLE_RINGS;
            break;
        default:
            std::cout << "default case activated" << std::endl;
            break;

    }

    // define center point
    glm::vec3 center_point = glm::vec3(frustum_corners[1].x - frustum_corners[0].x,
                                       frustum_corners[0].y - frustum_corners[3].y,
                                        0.0f);

    float angle = 360.0f / SAMPLE_RING_POINTS;

    std::vector<glm::vec3> vertices;
    vertices.push_back(center_point);

    for (int ring_idx = 0; ring_idx < SAMPLE_RINGS; ++ring_idx) {
        for (int ring_point_idx = 0; ring_point_idx < SAMPLE_RING_POINTS; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, angle * ring_point_idx));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx, step_size, 0.0);
            vertices.push_back(coord);
        }
    }

    return vertices;
}


/**
 * calc the angle between two vectors
 * @param a
 * @param b
 * @return
 */
float angle(glm::vec3 a, glm::vec3 b) {
    return acos(glm::dot(a,b) / (glm::length(a) * glm::length(b)));
}


/**
 * main function
 * @return
 */
int main() {

    std::cout << "Raycast" << std::endl;

    // create mirror & dome
    Sphere *mirror = new Sphere(0.8, glm::vec3(0.0, 0.5, 0.7));
    Sphere *dome = new Sphere(1.6, glm::vec3(0.0, 1.4, 0.0));

    // create projection matrix and calculate frustum corners
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0, 9.0, 0.0),   // camera pos world space
            glm::vec3(0.0, 0.0, -1.0),  // lookat
            glm::vec3(0.0, 1.0, 0.0)    // up vec
    );
    // calculate PV mat
    glm::mat4 pv = projection * view;

    // calculate frustum corners
    std::vector<glm::vec3> frustum_corners;
    calculateFrustumCorners(&frustum_corners, projection);
    for (unsigned int i = 0; i < frustum_corners.size(); ++i) {
        std::cout << vecstr(frustum_corners[i]) << std::endl;
    }

    // genrate radial grid
    std::vector<glm::vec3> radial_grid = generateRadialGrid(X, frustum_corners);
    std::cout << "Radial grid points" << std::endl;
    for (unsigned int i = 0; i < radial_grid.size(); ++i) {
        std::cout << vecstr(radial_grid[i]) << std::endl;
    }


    return 0;
}

