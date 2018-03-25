//
// Created by Hagen Hiller on 09/01/18.
//

#ifndef RAYCAST_DOMEPROJECTOR_HPP
#define RAYCAST_DOMEPROJECTOR_HPP


#include <vector>
#include <map>
#include <ostream>
#include <limits>

#include <glm/glm.hpp>

#include "projector_frustum.h"
#include "Frustum.hpp"
#include "Sphere.hpp"

class DomeProjector {

public:

    /**
     * Creates a dome projector object alongside the specified sample grid
     * @param _frustum
     * @param _screen
     * @param _grid_rings
     * @param _grid_ring_elements
     */
    DomeProjector(ProjectorFrustum *_frustum, int _grid_rings, int _grid_ring_elements,
                  glm::vec3 const &position, int dome_rings, int dome_ring_elements);

    /**
     * Destructor
     */
    ~DomeProjector();

    /**
     * calculate the transformation mesh
     * @return std::vector<glm::vec3> transformation mesh
     */
    std::vector<glm::vec3> calculateTransformationMesh();

    /**
     * calculates hitpoints in the dome
     * @param mirror
     * @param dome
     */
    void calculateDomeHitpoints(Sphere *mirror, Sphere *dome);


    /**
     * Returns the projectors frustum
     * @return
     */
    ProjectorFrustum* getFrustum() const;

    /**
     * Returns a std::vec containing the radial sample grid.
     * @return
     */
    std::vector<glm::vec3> const &get_sample_grid() const;

    /**
     * Returns a std::vector containing all first hitpoints supposed to be on the mirrors surface.
     * @return
     */
    std::vector<glm::vec3> const &get_first_hits() const;

    /**
     * Returns a std::vector containing all second hitpoints supposed to be within the dome.
     * @return
     */
    std::vector<glm::vec3> const &get_second_hits() const;

    /**
     * Returns a std::vector containing the vertices for a half sphere.
     * @return
     */
    std::vector<glm::vec3> const &get_dome_vertices() const;

    /**
     * Returns a std::vector containing vertices of the final warping mesh
     * @return
     */
    std::vector<glm::vec3> const &get_screen_points() const;

    /**
     * Returns a std::vector containing vertices of the final warping mesh
     * @return
     */
    std::vector<glm::vec3> const &get_texture_coords() const;

    /**
     * ostream
     * @param os
     * @param projector
     * @return
     */
    friend std::ostream &operator<<(std::ostream &os, const DomeProjector &projector);

private:

    /**
     * generates a radial grid
     * @return
     */
    void generateRadialGrid();

    /**
     * Generates the vertices of a half sphere by using the grid specified settings
     */
    void generateDomeVertices();

    // members
    ProjectorFrustum *_frustum;

    glm::vec3 _position;

    int _grid_rings;
    int _grid_ring_elements;

    int _dome_rings;
    int _dome_ring_elements;

    std::vector<glm::vec3> _sample_grid;
    std::vector<glm::vec3> _first_hits;
    std::vector<glm::vec3> _second_hits;

    std::vector<glm::vec3> _dome_vertices;

    std::vector<glm::vec3> _screen_points;
    std::vector<glm::vec3> _texture_coords;
};


#endif //RAYCAST_DOMEPROJECTOR_HPP
