//
// Created by Hagen Hiller on 09/01/18.
//

#ifndef RAYCAST_COLOR_HPP
#define RAYCAST_COLOR_HPP

struct ColorRGB {

    /**
     * Constructor accepting values in a range from 0..255.
     *
     * Float values are converted automatically.
     * @param r_i
     * @param g_i
     * @param b_i
     */
    ColorRGB(int r_i, int g_i, int b_i);

    /**
     * Constructor accepting values in a range from 0.0 .. 1.0f.
     *
     * Integer values from 0..255 are converted automatically.
     * @param r_f
     * @param g_f
     * @param b_f
     */
    ColorRGB(float r_f, float g_f, float b_f);

    // members
    unsigned int r_i;
    unsigned int g_i;
    unsigned int b_i;

    float r_f;
    float g_f;
    float b_f;
};


#endif //RAYCAST_COLOR_HPP
