//
// Created by Hagen Hiller on 09/01/18.
//

#include <glm/gtc/quaternion.hpp>
#include "DomeProjector.hpp"
#include "Utility.hpp"
#include "Sphere.hpp"

/**
 * Creates a dome projector object
 * @param _frustum
 * @param _screen
 */
DomeProjector::DomeProjector(Frustum *_frustum, Screen *_screen)
        : _frustum(_frustum)
        , _screen(_screen) {
}


/**
 * Creates a dome projector object alongside the specified sample grid
 * @param _frustum
 * @param _screen
 * @param _grid_rings
 * @param _grid_ring_elements
 */
DomeProjector::DomeProjector(Frustum *_frustum,
                             Screen *_screen,
                             int _grid_rings,
                             int _grid_ring_elements,
                             glm::vec3 const &position)
        : _frustum(_frustum)
        , _screen(_screen)
        , _grid_rings(_grid_rings)
        , _grid_ring_elements(_grid_ring_elements)
        , _position(position) {

    this->generateRadialGrid();
    this->generateDomeVertices();

    // translate sample grid to prosjector position
    for (int i = 0; i < this->_sample_grid.size(); ++i) {
        this->_sample_grid[i] = this->_sample_grid[i] + position;
    }

}


/**
 * Destructor
 */
DomeProjector::~DomeProjector() {
    delete this->_frustum;
    delete this->_screen;
}

/**
 * generates a radial grid
 * @return
 */
std::vector<glm::vec3> DomeProjector::generateRadialGrid() const {

    float step_size = ((this->_frustum->_far_clipping_corners[0].x - this->_frustum->_far_clipping_corners[1].x) / 2) /
                      _grid_rings;

    std::cout << "step size: " << step_size << std::endl;

    // define center point
    glm::vec3 center_point = glm::vec3(
            this->_frustum->_far_clipping_corners[1].x + this->_frustum->_far_clipping_corners[0].x,
            this->_frustum->_far_clipping_corners[0].y + this->_frustum->_far_clipping_corners[3].y,
            this->_frustum->_far_clipping_corners[0].z);

    float angle = 360.0f / _grid_ring_elements;
    std::cout << "partial angle: " << angle << std::endl;

    std::vector<glm::vec3> vertices;
    vertices.push_back(center_point);

    for (int ring_idx = 1; ring_idx < _grid_rings + 1; ++ring_idx) {
        for (int ring_point_idx = 0; ring_point_idx < _grid_ring_elements; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, glm::radians(angle * ring_point_idx)));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx * step_size, 0.0, 0.0);

            // push back the rotated point at the far clipping plaes z position
            vertices.push_back(glm::vec3(coord.x, coord.y, this->_frustum->_far_clipping_corners[0].z));
        }
    }

    return vertices;
}


/**
 * generates a radial grid
 * @return
 */
void DomeProjector::generateRadialGrid() {

    // if there are elements in there kill'em
    if (!this->_sample_grid.empty()) {
        this->_sample_grid.clear();
    }

    float step_size = ((this->_frustum->_far_clipping_corners[0].x - this->_frustum->_far_clipping_corners[1].x) / 2) /
                      _grid_rings;

    // define center point
    glm::vec3 center_point = glm::vec3(
            this->_frustum->_far_clipping_corners[1].x + this->_frustum->_far_clipping_corners[0].x,
            this->_frustum->_far_clipping_corners[0].y + this->_frustum->_far_clipping_corners[3].y,
            this->_frustum->_far_clipping_corners[0].z);

    float angle = 360.0f / _grid_ring_elements;

    this->_sample_grid.push_back(center_point);
    for (unsigned int ring_idx = 1; ring_idx < _grid_rings + 1; ++ring_idx) {
        for (unsigned int ring_point_idx = 0; ring_point_idx < _grid_ring_elements; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, glm::radians(angle * ring_point_idx)));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx * step_size, 0.0, 0.0);

            // push back the rotated point at the far clipping plaes z position
            this->_sample_grid.push_back(glm::vec3(coord.x, coord.y, this->_frustum->_far_clipping_corners[0].z));
        }
    }

}


/**
 * Generates the vertices of a half sphere by using the grid specified settings
 */
void DomeProjector::generateDomeVertices() {

    /*
     * THETA - AROUND Y
     * PHI - X AND
     */
    float delta_theta = 360.0f / (float) (this->_grid_ring_elements);
    float delta_phi = 90.0f / (float) (this->_grid_ring_elements - 1);

    // define center point
    std::vector<glm::vec3> vertices;
    glm::vec3 pole_cap(0.0f, 1.0f, 0.0f);
    vertices.push_back(pole_cap);

    for (int ring_idx = 0; ring_idx < this->_grid_rings; ++ring_idx) {
        glm::quat phi_quat(glm::vec3(glm::radians(ring_idx * delta_phi), 0.0f, 0.0f));
        glm::vec3 vec = phi_quat * glm::vec3(pole_cap.x, pole_cap.y, pole_cap.z);
        for (int segment_idx = 0; segment_idx < this->_grid_ring_elements; ++segment_idx) {
            glm::quat theta_quat(glm::vec3(0.0f, glm::radians(segment_idx * delta_theta), 0.0f));
            glm::vec3 final = theta_quat * vec;
            vertices.push_back(final);
        }
    }

    this->_dome_vertices = vertices;

}


/**
 * calculates hitpoints in the dome
 * @param mirror
 * @param dome
 */
void DomeProjector::calculateDomeHitpoints(Sphere *mirror, Sphere *dome) {

    // translate dome vertices to the domes position
    for (int i = 0; i < this->_dome_vertices.size(); ++i) {
        this->_dome_vertices[i] += dome->get_position();
        //todo fix scale
    }

    // raycast for each samplepoint
    // todo ignore points outside of the frustum
    for (int i = 0; i < this->_sample_grid.size(); ++i) {

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
                }
            }

        }

    }
}

/**
 * calculate the transformation mesh
 * @return std::vector<glm::vec3> transformation mesh
 */
std::vector<glm::vec3> DomeProjector::calculateTransformationMesh() {


    return std::vector<glm::vec3>();
}

/**
 * Updates the frustum and recalculates the sample points.
 * @param new_frustum
 */
void DomeProjector::updateFrustum(Frustum *new_frustum) {
    this->_frustum = new_frustum;
    this->generateRadialGrid();
}

/**
 *
 * @param new_screen
 */
void DomeProjector::updateScreen(Screen *new_screen) {
    this->_screen = new_screen;
}

/**
 * recalculate the radial grid
 */
void DomeProjector::updateSampleGrid(int, int) {
    this->generateRadialGrid();
}


/**
 * Updates the Dome Projectors position and recalculates all samplepoints;
 * @param new_position
 */
void DomeProjector::updatePosition(glm::vec3 const &new_position) {
    this->_position = new_position;
    this->generateRadialGrid();
}


// ---------------------------------------------------------------------------
// GETTER
// ---------------------------------------------------------------------------

/**
 * returns the amount of samplepoints
 * @return
 */
unsigned int DomeProjector::getNumSamplepoints() const {
    return (unsigned int) this->_sample_grid.size();
}

/**
 * returns the amount of total hits
 * @return
 */
unsigned int DomeProjector::getTotalHitCount() const {
    return (unsigned int) this->_second_hits.size();
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
 * ostream
 * @param os
 * @param projector
 * @return
 */
std::ostream &operator<<(std::ostream &os, const DomeProjector &projector) {
    os << "Dome Projector:" << "\n"
       << "  screen: <width: " << projector._screen->width << " height: " << projector._screen->height << ">\n"
       << "  position: [" << projector._position.x << ", " << projector._position.y << ", " << projector._position.z
       << "]\n"
       << "  grid_rings: " << projector._grid_rings << "\n"
       << "  grid_ring_elements: " << projector._grid_ring_elements << "\n"
       << "  sample_grid: " << projector._sample_grid.size() << "\n"
       << "  first_hits: " << projector._first_hits.size() << "\n"
       << "  second_hits: " << projector._second_hits.size();
    return os;
}


