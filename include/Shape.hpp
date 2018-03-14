//
// Created by Hagen Hiller on 02/01/18.
//

#ifndef RAYCAST_SHAPE_HPP
#define RAYCAST_SHAPE_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Shape {

public:
    Shape();


private:

    std::vector<float> vertex_data;
    std::string const& name;
};


#endif //RAYCAST_SHAPE_HPP
