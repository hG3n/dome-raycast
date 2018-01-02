#include <iostream>
#include <vector>
#include <map>
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
#include "vertex_buffer_data.hpp"

// gl globals
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 800;
float FOV = 90.0f;
float NEAR_CLIPPING_PLANE = 0.1f;
float FAR_CLIPPING_PLANE = 10.0f;

GLFWwindow *window;

std::vector<GLuint> vertex_buffer_ids;
std::vector<GLuint> vertex_array_ids;
std::vector<GLuint> shader_program_ids;

std::map<std::string, GLuint> color_map;
std::vector<glm::vec3> global_frustum_corners;

// raycast globals
int SAMPLE_RINGS = 4;
int SAMPLE_RING_POINTS = 18;

std::vector<glm::vec3> radial_grid_positions;

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
            step_size = ((frustum_corners[0].y - frustum_corners[2].y) / 2) / SAMPLE_RINGS;
            break;
        default:
            std::cout << "default case activated" << std::endl;
            break;
    }

    std::cout << "step size: " << step_size << std::endl;

    // define center point
    glm::vec3 center_point = glm::vec3(frustum_corners[1].x + frustum_corners[0].x,
                                       frustum_corners[0].y + frustum_corners[3].y,
                                       0.0f);

    float angle = 360.0f / SAMPLE_RING_POINTS;
    std::cout << "partial angle: " << angle << std::endl;

    std::vector<glm::vec3> vertices;
    vertices.push_back(center_point);

    for (int ring_idx = 1; ring_idx < SAMPLE_RINGS + 1; ++ring_idx) {
        for (int ring_point_idx = 0; ring_point_idx < SAMPLE_RING_POINTS; ++ring_point_idx) {
            glm::quat euler_quat(glm::vec3(0.0, 0.0, utility::d2r(angle * ring_point_idx)));
            glm::vec3 coord = euler_quat * glm::vec3(ring_idx * step_size, 0.0, 0.0);

            vertices.push_back(coord);
        }
    }

    return vertices;
}


/**
 * raycast the shit out of the setup
 * @return
 */
int raycast() {

    std::cout << "Raycast" << std::endl;

    // create mirror & dome
    Sphere *mirror = new Sphere(0.8, glm::vec3(0.0, 0.5, 0.7));
    Sphere *dome = new Sphere(1.6, glm::vec3(0.0, 1.4, 0.0));

    // create projection matrix and calculate frustum corners
    glm::mat4 projection = glm::perspective(glm::radians(FOV),
                                            float(SCREEN_WIDTH) / float(SCREEN_HEIGHT),
                                            NEAR_CLIPPING_PLANE,
                                            FAR_CLIPPING_PLANE);
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0, 0.0, 1.0),  // camera pos world space
            glm::vec3(0.0, 0.0, 0.0),  // lookat
            glm::vec3(0.0, 1.0, 0.0)   // up vec
    );

    // calculate PV mat
    glm::mat4 pv = projection * view;

    // calculate frustum corners
    std::vector<glm::vec3> frustum_corners;
    calculateFrustumCorners(&frustum_corners, projection);

    global_frustum_corners = frustum_corners;

    // generate radial grid
    std::vector<glm::vec3> radial_grid = generateRadialGrid(X, frustum_corners);

    radial_grid_positions = radial_grid;

    return 0;
}


/**
 * initialize open gl context by creating a window and setting up general behaviour
 * @return
 */
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
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DomeRaycast", nullptr, nullptr);
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

    // ensure key input capturing is possible
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // further settings
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // accept fragment if closer to camera than former one
}


/**
 * cleanup gl stuff
 */
void cleanupGL() {

    // Cleanup VBO
    for (int i = 0; i < vertex_buffer_ids.size(); ++i) {
        glDeleteBuffers(1, &vertex_buffer_ids[i]);
    }

    for (int i = 0; i < vertex_array_ids.size(); ++i) {
        glDeleteVertexArrays(1, &vertex_array_ids[i]);
    }

    for (int i = 0; i < shader_program_ids.size(); ++i) {
        glDeleteProgram(shader_program_ids[i]);
    }

}


/**
 * draw a vertex array
 * @param matrix_id
 * @param vertex_buffer_id
 * @param mvp
 */
void
drawVertexArray(GLuint matrix_id, GLuint vertex_buffer_id, glm::mat4 mvp, int num_vertices, GLuint colorbuffer_id) {

    // send transformation to current shader
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

    // vertex attribute buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(
            0,                  // attribute 0 must match shader layout
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void *) 0          // array buffer offset
    );

    // color
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_id);
    glVertexAttribPointer(
            1,                  // attribute 0 must match shader layout
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void *) 0          // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, num_vertices); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);
}


/**
 * render
 * @param mvp
 */
void render(glm::mat4 mvp) {

    // get uniform locations
    GLint matrix_id = glGetUniformLocation(shader_program_ids[0], "MVP");

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use shader
        glUseProgram(shader_program_ids[0]);

        glm::mat4 current_mvp;
        for (int i = 0; i < global_frustum_corners.size(); ++i) {
            current_mvp = glm::translate(mvp, global_frustum_corners[i]);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));

            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["red"]);

        }

        for (int i = 0; i < radial_grid_positions.size(); ++i) {
            current_mvp = glm::translate(mvp, radial_grid_positions[i]);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));

            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["red"]);
        }

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

    raycast();

    initializeGLContext();

    // create buffers
    GLuint vertexbuffer_id;
    glGenBuffers(1, &vertexbuffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quad), vertex_buffer_data::quad, GL_STATIC_DRAW);
    vertex_buffer_ids.push_back(vertexbuffer_id);

    // generate vertex array
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    vertex_array_ids.push_back(vertex_array_id);

    static const GLfloat red_buffer[] = {
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0
    };
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(red_buffer), red_buffer, GL_STATIC_DRAW);
    color_map["red"] = colorbuffer;



    // load shaders
    GLuint program_id = LoadShaders("../shaders/simple.vert", "../shaders/simple.frag");
    shader_program_ids.push_back(program_id);



    // build model view projection matrix
    // Get a handle for our "MVP" uniform
    GLint matrix_id = glGetUniformLocation(program_id, "MVP");

    // projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(FOV),                        // fov
                                            float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), // aspect ratio
                                            NEAR_CLIPPING_PLANE,
                                            FAR_CLIPPING_PLANE);
    // camera matrix
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 1.0f),  // camera position world space
            glm::vec3(0.0f, 0.0f, 0.0f),  // lookat
            glm::vec3(0.0f, 1.0f, 0.0f)   // upvec
    );

    // model
    glm::mat4 model = glm::mat4(1.0f);

    // MVP
    glm::mat4 mvp = projection * view * model;

    // -----------------------
    // DRAW
    render(mvp);
    // -----------------------

    cleanupGL();
    glfwTerminate();

}

