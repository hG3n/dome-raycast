//
// Created by Hagen Hiller on 09/01/18.
//

#include <Utility.hpp>
#include <iostream>
#include "ColorRGB.hpp"

ColorRGB::ColorRGB(int r_i, int g_i, int b_i)
        : r_i(r_i)
        , g_i(g_i)
        , b_i(b_i) {

    r_f = utility::mapToRange(r_i, 0.0f, 255.0f, 0.0f, 1.0f);
    g_f = utility::mapToRange(g_i, 0.0f, 255.0f, 0.0f, 1.0f);
    b_f = utility::mapToRange(b_i, 0.0f, 255.0f, 0.0f, 1.0f);
//
//    std::cout << r_i << " " << r_f << std::endl;
//    std::cout << g_i << " " << g_f << std::endl;
//    std::cout << b_i << " " << b_f << std::endl;
//
//    std::cout << "__color" << std::endl;
}

ColorRGB::ColorRGB(float r_f, float g_f, float b_f)
        : r_f(r_f)
        , g_f(g_f)
        , b_f(b_f) {

    r_i = (unsigned int) utility::mapToRange(r_f, 0.0f, 1.0f, 0.0f, 255.0f);
    g_i = (unsigned int) utility::mapToRange(g_f, 0.0f, 1.0f, 0.0f, 255.0f);
    b_i = (unsigned int) utility::mapToRange(b_f, 0.0f, 1.0f, 0.0f, 255.0f);

    std::cout << r_f << " " << r_i << std::endl;
    std::cout << g_f << " " << g_i << std::endl;
    std::cout << b_f << " " << b_i << std::endl;

}

