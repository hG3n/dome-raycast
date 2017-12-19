#include <iostream>


#include "Sphere.hpp"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>

int main() {

    std::cout << "Raycast" << std::endl;

    // create new sphere
    Sphere s = Sphere();
    std::cout << s << std::endl;

    glm::vec3 a(1,2,3);
    glm::vec3 b(2,3,4);

    float r = glm::dot(a,b);

    std::cout << r << std::endl;

    return 0;
}

