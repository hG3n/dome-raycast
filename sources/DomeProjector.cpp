//
// Created by Hagen Hiller on 09/01/18.
//

#include <glm/gtc/quaternion.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <lib/json11.hpp>
#include "DomeProjector.hpp"
#include "Utility.hpp"
#include "Sphere.hpp"

DomeProjector::DomeProjector(Frustum *_frustum,
                             int _grid_rings,
                             int _grid_ring_elements,
                             glm::vec3 const &position,
                             int dome_rings,
                             int dome_ring_elements)
        : _frustum(_frustum)
        , _grid_rings(_grid_rings)
        , _grid_ring_elements(_grid_ring_elements)
        , _position(position)
        , _dome_rings(dome_rings)
        , _dome_ring_elements(dome_ring_elements) {

    this->generateRadialGrid();
    this->generateDomeVertices();

    glm::mat4 scale_mat(1.0f);
    scale_mat = glm::scale(scale_mat, glm::vec3(1.6f, 1.6f, 1.6f));

    for(auto &vert: _dome_vertices) {
        vert = scale_mat * glm::vec4(vert, 1.0f);
    }

//    for (int j = 0; j < this->_dome_vertices.size(); ++j) {
//        glm::vec4 res = scale_mat * glm::vec4(this->_dome_vertices[j], 1.0f);
//        this->_dome_vertices[j] = glm::vec3(res);
//    }

    // translate sample grid to projector position
    // ignore the first element which is already in the frustums center
    for (int i = 1; i < this->_sample_grid.size(); ++i) {
        this->_sample_grid[i] = this->_sample_grid[i] + position;
    }

}


DomeProjector::~DomeProjector() {
    delete this->_frustum;
}

std::vector<glm::vec3> DomeProjector::calculateTransformationMesh() {

    // create green list here
    std::map<int, int> map;

    float last_distance = std::numeric_limits<float>::max();
    int last_hitpoint_idx = 0;

    for (int vert_idx = 0; vert_idx < this->_dome_vertices.size(); ++vert_idx) {
        for (int hp_idx = 0; hp_idx < this->_second_hits.size(); ++hp_idx) {
            float current_distance = glm::length(this->_second_hits[hp_idx] - this->_dome_vertices[vert_idx]);
            if (current_distance < last_distance) {
                last_distance = current_distance;
                last_hitpoint_idx = hp_idx;
            }
        }
        last_distance = std::numeric_limits<float>::max();
        map.insert(std::pair<int, int>(vert_idx, last_hitpoint_idx));
    }

    // calculate mapping
    std::vector<glm::vec3> screen_points;
    std::vector<glm::vec3> texture_points;
    for (auto pair : map) {
        std::cout << "dome: " << pair.first << " texture " << pair.second << std::endl;
        texture_points.push_back(this->_dome_vertices[pair.first]);
        screen_points.push_back(this->_sample_grid[pair.second]);
    }

    // normalize screen list
    float screen_min_x = utility::findMinValues(screen_points).x;
    float screen_max_x = utility::findMaxValues(screen_points).x;
    float screen_min_y = utility::findMinValues(screen_points).y;
    float screen_max_y = utility::findMaxValues(screen_points).y;

    std::vector<glm::vec3> screen_points_normalized;
    for (auto point : screen_points) {
        float new_x = utility::mapToRange(point.x,
                                          screen_min_x, screen_max_x,
                                          -1.0f, 1.0f);

        float new_y = utility::mapToRange(point.y,
                                          screen_min_y, screen_max_y,
                                          -1.0f, 1.0f);

        screen_points_normalized.emplace_back(glm::vec3(new_x, new_y, 0.0f));
    }

    // normalize texture points
    float texture_min_x = utility::findMinValues(texture_points).x;
    float texture_max_x = utility::findMaxValues(texture_points).x;
    float texture_min_z = utility::findMinValues(texture_points).z;
    float texture_max_z = utility::findMaxValues(texture_points).z;

    std::vector<glm::vec3> texture_coords_normalized;
    for (auto point:texture_points) {
        float new_x = utility::mapToRange(point.x,
                                          texture_min_x, texture_max_x,
                                          0.0f, 1.0f);

        float new_y = utility::mapToRange(point.z,
                                          texture_min_z, texture_max_z,
                                          0.0f, 1.0f);

        texture_coords_normalized.emplace_back(glm::vec3(new_x, new_y, 0.0f));
    }

    this->_screen_points = screen_points_normalized;
    this->_texture_coords = texture_coords_normalized;

    return std::vector<glm::vec3>();
}

void DomeProjector::calculateDomeHitpoints(Sphere *mirror, Sphere *dome) {

    this->_first_hits.clear();
    this->_second_hits.clear();

    // translate dome vertices to the domes position
    for (int i = 0; i < this->_dome_vertices.size(); ++i) {
        this->_dome_vertices[i] += dome->get_position();
        // todo move translation to scaling
    }

    // raycast for each samplepoint
    for (int i = 0; i < this->_sample_grid.size(); ++i) {

        // todo ignore points outside of the frustum
//        if (this->_sample_grid[i].y > _frustum->_near_clipping_corners[3].y &&
//            this->_sample_grid[i].y < _frustum->_near_clipping_corners[0].y) {
//            break;
//        }

        // calculate initial ray direction
        glm::vec3 initial_direction = this->_sample_grid[i] - this->_position;

        // build ray and define hitpoint
        Ray r(this->_position, glm::normalize(initial_direction));
        std::pair<Hitpoint, Hitpoint> hpp;
        if (mirror->intersect(r, &hpp)) {

            this->_first_hits.push_back(hpp.first.position);

            // reflect ray
            glm::vec3 ref = r.reflect(hpp.first.normal);

            Ray r2(hpp.first.position, glm::normalize(ref));
            std::pair<Hitpoint, Hitpoint> hpp2;
            if (dome->intersect(r2, &hpp2)) {
                if (hpp2.second.position.y > dome->get_position().y) {
                    this->_second_hits.push_back(hpp2.second.position);
                } else {
                    this->_second_hits.push_back({1000, 1000, 1000});
                }
            } else {
                this->_second_hits.push_back({1000, 1000, 1000});
            }

        } else {
            this->_second_hits.push_back({1000, 1000, 1000});
        }

    }

}

void DomeProjector::generateRadialGrid() {

    // if there are elements in there kill'em
    if (!this->_sample_grid.empty()) {
        this->_sample_grid.clear();
    }

    float step_size =
            ((this->_frustum->_near_clipping_corners[0].x - this->_frustum->_near_clipping_corners[1].x) / 2) /
            _grid_rings;

    // define center point
    glm::vec3 center_point = glm::vec3(
            (this->_frustum->_near_clipping_corners[1].x + this->_frustum->_near_clipping_corners[0].x) / 2,
            (this->_frustum->_near_clipping_corners[1].y + this->_frustum->_near_clipping_corners[3].y) / 2,
            this->_frustum->_near_clipping_corners[0].z);


    std::cout << utility::vecstr(center_point) << std::endl;

    float angle = 360.0f / _grid_ring_elements;

    this->_sample_grid.push_back(center_point);
    for (unsigned int ring_idx = 1; ring_idx < _grid_rings + 1; ++ring_idx) {
        for (unsigned int ring_point_idx = 0; ring_point_idx < _grid_ring_elements; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, glm::radians(angle * ring_point_idx)));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx * step_size, 0.0, 0.0);

            // push back the rotated point at the near clipping plaes z position
            this->_sample_grid.emplace_back(glm::vec3(coord.x, coord.y, this->_frustum->_near_clipping_corners[0].z));
        }
    }

}

void DomeProjector::generateDomeVertices() {
    /*
     * THETA - AROUND Y
     * PHI - X AND
     */
    float delta_theta = 360.0f / (float) (this->_dome_ring_elements);
    float delta_phi = 90.0f / (float) (this->_dome_rings);

    // define center point
    std::vector<glm::vec3> vertices;
    glm::vec3 pole_cap(0.0f, 1.0f, 0.0f);
    vertices.push_back(pole_cap);

    for (int ring_idx = 1; ring_idx < this->_dome_rings + 1; ++ring_idx) {
        glm::quat phi_quat(glm::vec3(glm::radians(ring_idx * delta_phi), 0.0f, 0.0f));
        glm::vec3 vec = phi_quat * glm::vec3(pole_cap.x, pole_cap.y, pole_cap.z);
        for (int segment_idx = 0; segment_idx < this->_dome_ring_elements; ++segment_idx) {
            glm::quat theta_quat(glm::vec3(0.0f, glm::radians(segment_idx * delta_theta), 0.0f));
            glm::vec3 final = theta_quat * vec;
            vertices.push_back(final);
        }
    }

    this->_dome_vertices = vertices;
}

/**
 * Returns a std::vec containing the radial sample grid.
 * @return
 */
std::vector<glm::vec3> const &DomeProjector::get_sample_grid() const {
    return this->_sample_grid;
}

/**
 * Returns a std::vector containing all first hitpoints supposed to be on the mirrors surface.
 * @return
 */
std::vector<glm::vec3> const &DomeProjector::get_first_hits() const {
    return this->_first_hits;
}

/**
 * Returns a std::vector containing all second hitpoints supposed to be within the dome.
 * @return
 */
std::vector<glm::vec3> const &DomeProjector::get_second_hits() const {
    return this->_second_hits;
}

/**
 * Returns a std::vector containing the vertices for a half sphere.
 * @return
 */
std::vector<glm::vec3> const &DomeProjector::get_dome_vertices() const {
    return this->_dome_vertices;
}

/**
 * Returns a std::vector containing vertices of the final warping mesh
 * @return
 */
std::vector<glm::vec3> const &DomeProjector::get_screen_points() const {
    return this->_screen_points;
}

/**
 * Returns a std::vector containing vertices of the final warping mesh
 * @return
 */
std::vector<glm::vec3> const &DomeProjector::get_texture_coords() const {
    return this->_texture_coords;
}

/**
 * ostream
 * @param os
 * @param projector
 * @return
 */
std::ostream &operator<<(std::ostream &os, const DomeProjector &projector) {
    os << "Dome Projector:" << "\n"
       << "  position: [" << projector._position.x << ", " << projector._position.y << ", " << projector._position.z
       << "]\n"
       << "  grid_rings: " << projector._grid_rings << "\n"
       << "  grid_ring_elements: " << projector._grid_ring_elements << "\n"
       << "  sample_grid: " << projector._sample_grid.size() << "\n"
       << "  first_hits: " << projector._first_hits.size() << "\n"
       << "  second_hits: " << projector._second_hits.size();
    return os;
}


