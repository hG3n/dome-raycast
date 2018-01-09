#version 330 core

// Ouput data
out vec4 color;
//out vec3 color;

// input color
in vec3 fragment_color;

/**
 * main
 */
void main() {
    color = vec4(fragment_color, 1.0);
//    color = fragment_color;
}
