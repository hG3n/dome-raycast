#version 330 core

// Ouput data
out vec4 color;

// input color
in vec3 fragment_color;

/**
 * main
 */
void main() {
    color = vec4(fragment_color, 1.0);
}
