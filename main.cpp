// include std headers
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Sphere.hpp"
#include "ShaderUtil.hpp"
#include "Utility.hpp"

// gl globals
GLFWwindow *window;

int SAMPLE_RINGS = 10;
int SAMPLE_RING_POINTS = 36;

enum Axis {
    X, Y, Z
};

/**
 * calculate frustum corners according to the current projection matrix
 * @param corners
 * @param projection
 */
void calculateFrustumCorners(std::vector<glm::vec3> *corners, glm::mat4 projection) {

    glm::vec4 hcorners[8];

    // near
    hcorners[0] = glm::vec4(-1, 1, 1, 1);
    hcorners[1] = glm::vec4(1, 1, 1, 1);
    hcorners[2] = glm::vec4(1, -1, 1, 1);
    hcorners[3] = glm::vec4(-1, -1, 1, 1);

    // far
    hcorners[4] = glm::vec4(-1, 1, -1, 1);
    hcorners[5] = glm::vec4(1, 1, -1, 1);
    hcorners[6] = glm::vec4(1, -1, -1, 1);
    hcorners[7] = glm::vec4(-1, -1, -1, 1);

    glm::mat4 inv_proj = glm::inverse(projection);

    for (int i = 0; i < hcorners->length(); ++i) {
        hcorners[i] = hcorners[i] * inv_proj;
        hcorners[i] /= hcorners[i].w;

        corners->push_back(glm::vec3(hcorners[i]));
    }
}




/**
 * generates a radial grid along given axis
 * @param axis
 * @return vector containing grid points
 */
std::vector<glm::vec3> generateRadialGrid(Axis axis, std::vector<glm::vec3> frustum_corners) {

    float step_size = 0;
    switch (axis) {

        case X:
            step_size = ((frustum_corners[0].x - frustum_corners[1].x) / 2) / SAMPLE_RINGS;
            break;
        case Y:
            step_size = ((frustum_corners[0].y - frustum_corners[3].y) / 2) / SAMPLE_RINGS;
            break;
        default:
            std::cout << "default case activated" << std::endl;
            break;

    }

    // define center point
    glm::vec3 center_point = glm::vec3(frustum_corners[1].x - frustum_corners[0].x,
                                       frustum_corners[0].y - frustum_corners[3].y,
                                        0.0f);

    float angle = 360.0f / SAMPLE_RING_POINTS;

    std::vector<glm::vec3> vertices;
    vertices.push_back(center_point);

    for (int ring_idx = 0; ring_idx < SAMPLE_RINGS; ++ring_idx) {
        for (int ring_point_idx = 0; ring_point_idx < SAMPLE_RING_POINTS; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, angle * ring_point_idx));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx, step_size, 0.0);
            vertices.push_back(coord);
        }
    }

    return vertices;
}




int foo() {

    std::cout << "Raycast" << std::endl;

    // create mirror & dome
    Sphere *mirror = new Sphere(0.8, glm::vec3(0.0, 0.5, 0.7));
    Sphere *dome = new Sphere(1.6, glm::vec3(0.0, 1.4, 0.0));

    // create projection matrix and calculate frustum corners
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0, 9.0, 0.0),   // camera pos world space
            glm::vec3(0.0, 0.0, -1.0),  // lookat
            glm::vec3(0.0, 1.0, 0.0)    // up vec
    );
    // calculate PV mat
    glm::mat4 pv = projection * view;

    // calculate frustum corners
    std::vector<glm::vec3> frustum_corners;
    calculateFrustumCorners(&frustum_corners, projection);
    for (unsigned int i = 0; i < frustum_corners.size(); ++i) {
        std::cout << utility::vecstr(frustum_corners[i]) << std::endl;
    }

    // generate radial grid
    std::vector<glm::vec3> radial_grid = generateRadialGrid(X, frustum_corners);
    std::cout << "Radial grid points" << std::endl;
    for (unsigned int i = 0; i < radial_grid.size(); ++i) {
        std::cout << utility::vecstr(radial_grid[i]) << std::endl;
    }


    return 0;
}

int initializeGLContext() {

    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // get fullscreen resolution
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int width = mode->width;
    int height = mode->height;

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1280, 800, "GLWarp", nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr,
                "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // further settings
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // accept fragment if closer to camera than former one
}

void cleanupGL() {

//    // Cleanup VBO
//    glDeleteBuffers(1, &vertexbuffer_id);
//    glDeleteBuffers(1, &uv_buffer_id);
//    glDeleteProgram(program_id);
//    glDeleteTextures(1, &texture);
//    glDeleteVertexArrays(1, &vertex_array_id);

}

void draw() {
        /**
     * draw
     */
    bool running = true;
    double last_time = glfwGetTime();
    int num_frames = 0;
    while (running && glfwWindowShouldClose(window) == 0) {


        double current_time = glfwGetTime();
        ++num_frames;
        if (current_time - last_time >= 1.0) {
            std::cout << "ms/frame: " << (1000.0 / double(num_frames)) << std::endl;
            num_frames = 0;
            last_time += 1.0;
        }

        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        // use shader
//        glUseProgram(program_id);
//
//        // send transformation to current shader
//        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
//
//        // textureshit
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture);
//        glUniform1i(texture_id, 0);
//
//        // vertex attribute buffer
//        glEnableVertexAttribArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_id);
//        glVertexAttribPointer(
//                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//                3,                  // size
//                GL_FLOAT,           // type
//                GL_FALSE,           // normalized?
//                0,                  // stride
//                (void *) 0            // array buffer offset
//        );
//
//        // UV attribute buffer
//        glEnableVertexAttribArray(1);
//        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
//        glVertexAttribPointer(
//                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
//                2,                                // size : U+V => 2
//                GL_FLOAT,                         // type
//                GL_FALSE,                         // normalized?
//                0,                                // stride
//                (void*)0                          // array buffer offset
//        );
//
//        glDrawArrays(GL_TRIANGLES, 0, 12*3); // 3 indices starting at 0 -> 1 triangle
//
//        glDisableVertexAttribArray(0);
//        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // check for keyboard input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            running = false;
        }
    }
}


/**
 * main function
 * @return
 */
int main() {

    initializeGLContext();

    // furtehr setup shit

    draw();

    cleanupGL();
    glfwTerminate();

}

