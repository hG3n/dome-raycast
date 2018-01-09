//
// Created by Hagen Hiller on 09/01/18.
//

#ifndef RAYCAST_DOMEPROJECTOR_HPP
#define RAYCAST_DOMEPROJECTOR_HPP


#include <vector>
#include <glm/glm.hpp>
#include <ostream>

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

    DomeProjector(Frustum *_frustum, Screen *_screen);
    DomeProjector(Frustum *_frustum, Screen *_screen, int _grid_rings, int _grid_ring_elements, glm::vec3 const& position);
    ~DomeProjector();

    std::vector<glm::vec3> generateRadialGrid() const;
    std::vector<glm::vec3> calculateTransformationMesh();
    void calculateDomeHitpoints(Sphere* mirror, Sphere* dome);

    // setter
    void updateFrustum(Frustum *);
    void updateScreen(Screen *);
    void updateSampleGrid(int, int);
    void updatePosition(glm::vec3 const& new_position);

    // getter
    unsigned int getNumSamplepoints() const;
    unsigned int getTotalHitCount() const;
    std::vector<glm::vec3>const& get_sample_grid() const;
    std::vector<glm::vec3>const& get_first_hits() const;
    std::vector<glm::vec3>const& get_second_hits() const;
    std::vector<glm::vec3>const& get_dome_vertices() const;

    // ostream
    friend std::ostream &operator<<(std::ostream &os, const DomeProjector &projector);

private:

    void generateRadialGrid();
    void generateDomeVertices();

    // members
    Frustum *_frustum;
    Screen *_screen;

    glm::vec3 _position;

    int _grid_rings;
    int _grid_ring_elements;

    std::vector<glm::vec3> _sample_grid;
    std::vector<glm::vec3> _first_hits;
    std::vector<glm::vec3> _second_hits;

    std::vector<glm::vec3> _dome_vertices;

};


#endif //RAYCAST_DOMEPROJECTOR_HPP
