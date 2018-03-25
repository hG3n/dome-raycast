#include <glm/gtc/quaternion.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <lib/json11.hpp>
#include "DomeProjector.hpp"
#include "Utility.hpp"
#include "Sphere.hpp"

DomeProjector::DomeProjector(ProjectorFrustum *frustum,
                             int grid_rings,
                             int grid_ring_elements,
                             glm::vec3 const &position,
                             int dome_rings,
                             int dome_ring_elements)
        : _frustum(frustum)
        , _grid_rings(grid_rings)
        , _grid_ring_elements(grid_ring_elements)
        , _position(position)
        , _dome_rings(dome_rings)
        , _dome_ring_elements(dome_ring_elements) {


    this->generateRadialGrid();
    this->generateDomeVertices();

    glm::mat4 scale_mat(1.0f);
    scale_mat = glm::scale(scale_mat, glm::vec3(1.6f, 1.6f, 1.6f));

    for (auto &vert: _dome_vertices) {
        vert = scale_mat * glm::vec4(vert, 1.0f);
    }

    // translate frustum alongside
    _frustum->translateTo(position);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
    for (auto &point: _sample_grid) {
        point = glm::vec3(translation * glm::vec4(point, 1.0f));
    }

}


DomeProjector::~DomeProjector() {
    delete _frustum;
}

/*
 * methods
 */
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

    _first_hits.clear();
    _second_hits.clear();

    // translate dome vertices to the domes position
    glm::mat4 dome_translation = glm::translate(glm::mat4(1.0f), dome->get_position());
    for(auto &vertex : _dome_vertices) {
        vertex = glm::vec3(dome_translation * glm::vec4(vertex, 1.0f));
    }

    auto near_clipping_corners = _frustum->getNearCorners();

    // raycast for each samplepoint
    for (int i = 0; i < _sample_grid.size(); ++i) {

        auto current = _sample_grid[i];

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

    // if there are elements in there clear the vector
    if (!this->_sample_grid.empty()) {
        this->_sample_grid.clear();
    }

    auto near = _frustum->getNearCorners();

    float step_size = fabsf((near[ProjectorFrustum::TL].x - near[ProjectorFrustum::TR].x) / _grid_rings) / 2;

    // define center point
    float center_x = near[ProjectorFrustum::TR].x + near[ProjectorFrustum::TL].x;
    float center_y = near[ProjectorFrustum::TR].y + near[ProjectorFrustum::BL].y;
    float center_z = near[ProjectorFrustum::TL].z;
    glm::vec3 center_point(center_x, center_y, center_z);

    float step_angle = 360.0f / _grid_ring_elements;

    this->_sample_grid.push_back(center_point);
    for (unsigned int ring_idx = 1; ring_idx < _grid_rings + 1; ++ring_idx) {
        for (unsigned int ring_point_idx = 0; ring_point_idx < _grid_ring_elements; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, glm::radians(step_angle * ring_point_idx)));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx * step_size, 0.0, 0.0);

            float bottom_y = near[ProjectorFrustum::BR].y;
            float top_y = near[ProjectorFrustum::TL].y;
            if (coord.y < top_y && coord.y > bottom_y) {
                // push back the rotated point at the near clipping planes z position
                this->_sample_grid.emplace_back(glm::vec3(coord.x, coord.y, near[ProjectorFrustum::TL].z));
            }

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

/*
 * Getter
 */
ProjectorFrustum *DomeProjector::getFrustum() const {
    return _frustum;
}

std::vector<glm::vec3> const &DomeProjector::get_sample_grid() const {
    return this->_sample_grid;
}

std::vector<glm::vec3> const &DomeProjector::get_first_hits() const {
    return this->_first_hits;
}

std::vector<glm::vec3> const &DomeProjector::get_second_hits() const {
    return this->_second_hits;
}

std::vector<glm::vec3> const &DomeProjector::get_dome_vertices() const {
    return this->_dome_vertices;
}

std::vector<glm::vec3> const &DomeProjector::get_screen_points() const {
    return this->_screen_points;
}

std::vector<glm::vec3> const &DomeProjector::get_texture_coords() const {
    return this->_texture_coords;
}

/*
 * ostream
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


