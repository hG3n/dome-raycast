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


struct Screen {

    Screen(unsigned int width, unsigned int height)
            : width(width)
            , height(height) {}

    unsigned int width;
    unsigned int height;
};


class DomeProjector {

public:

    /**
     * Creates a dome projector object alongside the specified sample grid
     * @param _frustum
     * @param _screen
     * @param _grid_rings
     * @param _grid_ring_elements
     */
    DomeProjector(Frustum *_frustum, int _grid_rings, int _grid_ring_elements,
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

    // getter
    std::vector<glm::vec3> const &get_sample_grid() const;
    std::vector<glm::vec3> const &get_first_hits() const;
    std::vector<glm::vec3> const &get_second_hits() const;
    std::vector<glm::vec3> const &get_dome_vertices() const;
    std::vector<glm::vec3> const &get_screen_points() const;
    std::vector<glm::vec3> const &get_texture_coords() const;

    // ostream
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
    Frustum *_frustum;
    ProjectorFrustum __frustum;

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
