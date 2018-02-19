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

    DomeProjector(Frustum *_frustum, Screen *_screen, int _grid_rings, int _grid_ring_elements,
                  glm::vec3 const &position, int dome_rings, int dome_ring_elements);
    ~DomeProjector();

    std::vector<glm::vec3> calculateTransformationMesh();
    void calculateDomeHitpoints(Sphere *mirror, Sphere *dome);

    void saveTransformations() const;

    // setter
    void updateFrustum(Frustum *);
    void updateScreen(Screen *);
    void updateSampleGrid(int, int);
    void updatePosition(glm::vec3 const &new_position);

    // getter
    unsigned int getNumSamplepoints() const;
    unsigned int getTotalHitCount() const;
    std::vector<glm::vec3> const &get_sample_grid() const;
    std::vector<glm::vec3> const &get_first_hits() const;
    std::vector<glm::vec3> const &get_second_hits() const;
    std::vector<glm::vec3> const &get_dome_vertices() const;
    std::vector<glm::vec3> const &get_screen_points() const;
    std::vector<glm::vec3> const &get_texture_coords() const;

    // ostream
    friend std::ostream &operator<<(std::ostream &os, const DomeProjector &projector);


    std::vector<glm::vec3> corresponding_hitpoints;


private:

    void generateRadialGrid();
    void generateDomeVertices();

    // members
    Frustum *_frustum;
    Screen *_screen;

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
