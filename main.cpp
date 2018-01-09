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

#include "Sphere.hpp"
#include "ShaderUtil.hpp"
#include "DomeProjector.hpp"
#include "vertex_buffer_data.hpp"

// gl globals
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 800;
float FOV = 90.0f;
float NEAR_CLIPPING_PLANE = 0.1f;
float FAR_CLIPPING_PLANE = 10.0f;

bool SHOW_MOUSE = false;

GLFWwindow *window;

std::vector<GLuint> vertex_buffer_ids;
std::vector<GLuint> vertex_array_ids;
std::vector<GLuint> shader_program_ids;

std::map<std::string, GLuint> color_map;

// drawable globals
std::vector<glm::vec3> far_clipping_corners;
std::vector<glm::vec3> near_clipping_corners;
std::vector<glm::vec3> first_hitpoints;
std::vector<glm::vec3> second_hitpoints;
std::vector<glm::vec3> sample_grid;
std::vector<glm::vec3> origin;
std::vector<glm::vec3> dome_vertices;

// prepareRaycast globals
int SAMPLE_RINGS = 36;
int SAMPLE_RING_POINTS = 72;

GLuint createVertexBuffer(std::vector<GLfloat> const &vertex_data) {

    // create clike array to passit to gl
    GLfloat arr[vertex_data.size()];
    for (int i = 0; i < vertex_data.size(); ++i) {
        arr[i] = vertex_data[i];
    }

    // generate buffer
    GLuint vertexbuffer_id;
    glGenBuffers(1, &vertexbuffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STATIC_DRAW);

    return vertexbuffer_id;
}

GLuint createSolidColorBuffer(int size, float r, float g, float b) {

    std::cout << size << std::endl;
    GLfloat arr[size];
    std::cout << size / 3 << std::endl;
    for (int i = 0; i < size / 3; ++i) {
        arr[i * 2] = r;
        arr[i * 2 + 1] = g;
        arr[i * 3 + 2] = b;
        std::cout << r << " " << g << " " << b << std::endl;
    }

    GLuint colorbuffer_id;
    glGenBuffers(1, &colorbuffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STATIC_DRAW);

    return colorbuffer_id;
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

    // Hide the mouse and enable unlimited mouvement
    if (!SHOW_MOUSE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
            // this works better than expected
            std::cout << "\r";
            std::cout << "ms/frame: " << (1000.0 / double(num_frames));
            num_frames = 0;
            last_time += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use shader
        glUseProgram(shader_program_ids[0]);

        /*
         * keyboard input
         */
        glm::mat4 current_mvp;
        if (glfwGetKey(window, GLFW_KEY_A)) {
            mvp = glm::rotate(mvp, glm::radians(1.0f), glm::vec3(0, 1, 0));
        } else if (glfwGetKey(window, GLFW_KEY_D)) {
            mvp = glm::rotate(mvp, glm::radians(-1.0f), glm::vec3(0, 1, 0));
        }

        for (auto element : sample_grid) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["blue"]);
        }

        for (auto element : first_hitpoints) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["green"]);
        }

        for (auto element : second_hitpoints) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["red"]);
        }

        for (auto element: far_clipping_corners) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["yellow"]);
        }

        for (auto element: near_clipping_corners) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["yellow"]);
        }

        for (auto element: origin) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["yellow"]);
        }

        for (auto element: dome_vertices) {
            current_mvp = glm::translate(mvp, element);
            current_mvp = glm::scale(current_mvp, glm::vec3(0.01, 0.01, 0.01));
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["white"]);
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

    // create mirror & dome
    Sphere *mirror = new Sphere(0.4, glm::vec3(0.0, 0.8, -1.7));
    Sphere *dome = new Sphere(1.6, glm::vec3(0.0, 1.9, 0.0));
//    Sphere *mirror = new Sphere(1.5, glm::vec3(0.0, 1.0, -3.5));
//    Sphere *dome = new Sphere(1.0, glm::vec3(0.0, 1.0, 3.0));

//    // create projector_projection matrix and calculate frustum corners
    glm::mat4 projector_projection = glm::perspective(glm::radians(FOV),
                                                      float(SCREEN_WIDTH) / float(SCREEN_HEIGHT),
                                                      NEAR_CLIPPING_PLANE,
                                                      FAR_CLIPPING_PLANE);
//
//
//    // calculate initial ray direction
//    glm::vec3 initial_direction = glm::vec3(0, 0, -1) - glm::vec3(0, 0, 0);
//
//    // build ray and define hitpoint
//    Ray r(glm::vec3(0, 0, 0), glm::normalize(initial_direction));
//    std::pair<Hitpoint, Hitpoint> hpp;
//    std::cout << *mirror << std::endl;
//    std::cout << *dome << std::endl;
//    if (mirror->intersect(r, &hpp)) {
//
//        first_hitpoints.push_back(hpp.first.position);
//        std::cout << "hit sphere at   " << utility::vecstr(hpp.first.position) << std::endl;
//
//        // reflect ray
//        glm::vec3 ref = r.reflect(hpp.first.normal);
//
//        std::cout << "hitpoint normal " << utility::vecstr(hpp.first.normal) << std::endl;
//        std::cout << "reflected ray   " << utility::vecstr(ref) << std::endl;
//
////        Ray r2(hpp.first.position, glm::normalize(glm::normalize(ref)) + hpp.first.position);
//        Ray r2(hpp.first.position, glm::normalize(ref));
//        std::cout << r2 << std::endl;
//        std::pair<Hitpoint, Hitpoint> hpp2;
//        if (dome->intersect(r2, &hpp2)) {
//            std::cout << "fuckin hit at:" << utility::vecstr(hpp2.first.position) << std::endl;
//            second_hitpoints.push_back(hpp2.first.position);
//        }
//
//    }

    // view mat
    glm::vec3 projector_world_pos = glm::vec3(0.0, 0.9, 0.0);

    // build the dome projector
    Screen *screen = new Screen(SCREEN_WIDTH, SCREEN_HEIGHT);
    Frustum *frustum = new Frustum(projector_projection, projector_world_pos, true);
    DomeProjector *dp = new DomeProjector(frustum, screen, SAMPLE_RINGS, SAMPLE_RING_POINTS, projector_world_pos);

    // raycast the shit out of the setup
    dp->calculateDomeHitpoints(mirror, dome);
    dp->calculateTransformationMesh();

    std::cout << *dp << std::endl;

    far_clipping_corners = frustum->_far_clipping_corners;
    near_clipping_corners = frustum->_near_clipping_corners;
    first_hitpoints = dp->get_first_hits();
    second_hitpoints = dp->get_second_hits();
    sample_grid = dp->get_sample_grid();
    dome_vertices = dp->get_dome_vertices();

    // cleanup
//    delete dp;
    delete mirror;
    delete dome;

    initializeGLContext();

    // generate vertex array
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    vertex_array_ids.push_back(vertex_array_id);

    // create buffers
    vertex_buffer_ids.push_back(createVertexBuffer(vertex_buffer_data::quad));


    GLuint colorbuffer_blue;
    glGenBuffers(1, &colorbuffer_blue);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_blue);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quat_blue), vertex_buffer_data::quat_blue, GL_STATIC_DRAW);
    color_map["blue"] = colorbuffer_blue;

    GLuint colorbuffer_green;
    glGenBuffers(1, &colorbuffer_green);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_green);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quat_green), vertex_buffer_data::quat_green,
                 GL_STATIC_DRAW);
    color_map["green"] = colorbuffer_blue;

    GLuint colorbuffer_red;
    glGenBuffers(1, &colorbuffer_red);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_red);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quat_red), vertex_buffer_data::quat_red, GL_STATIC_DRAW);
    color_map["red"] = colorbuffer_red;

    GLuint colorbuffer_yellow;
    glGenBuffers(1, &colorbuffer_yellow);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_yellow);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quat_yellow), vertex_buffer_data::quat_yellow,
                 GL_STATIC_DRAW);
    color_map["yellow"] = colorbuffer_yellow;

    GLuint colorbuffer_white;
    glGenBuffers(1, &colorbuffer_white);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_white);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quat_white), vertex_buffer_data::quat_white,
                 GL_STATIC_DRAW);
    color_map["white"] = colorbuffer_white;

    color_map["grey"] = createSolidColorBuffer(vertex_buffer_data::quad.size(), 0.5, 0.5, 0.5);

    // load shaders
    GLuint program_id = LoadShaders("../shaders/simple.vert", "../shaders/simple.frag");
    shader_program_ids.push_back(program_id);

    // build model view projector_projection matrix
    // Get a handle for our "MVP" uniform
    GLint matrix_id = glGetUniformLocation(program_id, "MVP");

    origin.push_back(glm::vec3(0, 0, 0));

    // model
    // create projector_projection matrix and calculate frustum corners
    glm::mat4 camera_projection = glm::perspective(glm::radians(FOV),
                                                   float(SCREEN_WIDTH) / float(SCREEN_HEIGHT),
                                                   0.1f,
                                                   100.0f);

    // view mat
//    glm::vec3 camera_world_pos = glm::vec3(0.0, 1.0, -2.5);
//    glm::vec3 camera_lookat = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 camera_world_pos = glm::vec3(0.0, 1.0, -3.5);
    glm::vec3 camera_lookat = glm::vec3(0.0, 1.0, 0.0);
    glm::mat4 camera_view = glm::lookAt(
            camera_world_pos,           // camera pos world space
            camera_lookat,              // lookat
            glm::vec3(0.0, 1.0, 0.0)    // up vec
    );

    // model mat
    glm::mat4 model(1.0f);

    // MVP
    glm::mat4 mvp = camera_projection * camera_view * model;

    // -----------------------
    // DRAW
    render(mvp);
    // -----------------------

    cleanupGL();
    glfwTerminate();

}

