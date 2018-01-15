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
#include <sstream>

#include "json11.hpp"

#include "Sphere.hpp"
#include "ShaderUtil.hpp"
#include "DomeProjector.hpp"
#include "vertex_buffer_data.hpp"

// gl globals
GLFWwindow *window;
DomeProjector *dp;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 800;
float FOV = 90.0f;

bool SHOW_MOUSE = false;

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
std::vector<glm::vec3> screen_points;
std::vector<glm::vec3> texture_coords;

// raycast globals
int SAMPLE_RINGS = 72;
int SAMPLE_RING_POINTS = 72;
int DOME_RINGS = 18;
int DOME_RING_ELEMENTS = 36;

std::map<std::string, json11::Json> model_config;
std::map<std::string, json11::Json> application_config;

/**
 * extract json object from vector
 * @param vec_obj
 * @return
 */
glm::vec3 jsonArray2Vec3(json11::Json vec_obj) {
    std::vector<json11::Json> e = vec_obj.array_items();
    return glm::vec3(e[0].number_value(), e[1].number_value(), e[2].number_value());
}

/**
 * load json config
 * @param file_name
 * @param config
 * @return
 */
bool loadConfig(std::string const &file_name, json11::Json &config) {

    std::ifstream ifs(file_name);
    if (ifs.good()) {
        std::cout << "Loaded file '" << file_name << "' successfully" << std::endl;
    } else {
        return false;
    }

    // parse to string
    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    std::string error;
    auto json = json11::Json::parse(str, error);

    if (error.empty()) {
        std::cout << "Json parsing succeeded!" << std::endl;
        config = json;
    } else {
        std::cout << "Error loading json: " << error << std::endl;
        return false;
    }

    return true;
}

/**
 * create a vertex buffer
 * @param vertex_data
 * @return
 */
GLuint createVertexBuffer(std::vector<GLfloat> const &vertex_data) {

    // create clike array to pass it to gl
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


/**
 * create a solid color gl buffer
 * @param size
 * @param r
 * @param g
 * @param b
 * @return
 */
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
            drawVertexArray(matrix_id, vertex_array_ids[0], current_mvp, 2 * 3, color_map["grey"]);
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

void setupBuffers() {

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
    color_map["green"] = colorbuffer_green;

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

    GLuint colorbuffer_grey;
    glGenBuffers(1, &colorbuffer_grey);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_grey);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data::quat_grey), vertex_buffer_data::quat_grey,
                 GL_STATIC_DRAW);
    color_map["grey"] = colorbuffer_grey;

}

void runModelCalculations() {

    glm::vec3 mirror_position = jsonArray2Vec3(model_config["mirror"]["position"]);
    glm::vec3 dome_position = jsonArray2Vec3(model_config["dome"]["position"]);

    float mirror_radius = (float)model_config["mirror"]["radius"].number_value();
    float dome_radius = (float)model_config["dome"]["radius"].number_value();

    // create mirror & dome
    Sphere *mirror = new Sphere(mirror_radius, mirror_position);
    Sphere *dome = new Sphere(dome_radius, dome_position);

    float fov = (float) model_config["projector"]["fov"].number_value();
    int screen_width = (int) model_config["projector"]["screen"]["w"].number_value();
    int screen_height = (int) model_config["projector"]["screen"]["h"].number_value();

    glm::mat4 projector_projection = glm::perspective(glm::radians(fov),
                                                      float(screen_width) / float(screen_height),
                                                      0.1f,
                                                      10.0f);


    // view mat
    glm::vec3 projector_world_pos = jsonArray2Vec3(model_config["projector"]["position"]);

    int grid_rings = (int) model_config["projector"]["grid"]["num_rings"].number_value();
    int grid_ring_elements = (int) model_config["projector"]["grid"]["num_ring_elements"].number_value();
    int dome_rings = (int) model_config["projector"]["dome"]["num_rings"].number_value();
    int dome_ring_elements = (int) model_config["projector"]["dome"]["num_ring_elements"].number_value();

    // build the dome projector
    Screen *screen = new Screen(screen_width, screen_height);
    Frustum *frustum = new Frustum(projector_projection, projector_world_pos, true);
    dp = new DomeProjector(frustum,
                           screen,
                           grid_rings,
                           grid_ring_elements,
                           projector_world_pos,
                           dome_rings,
                           dome_ring_elements);

    dp->calculateDomeHitpoints(mirror, dome);
    dp->calculateTransformationMesh();
    dp->calculateTransformationMesh();

    far_clipping_corners = frustum->_near_clipping_corners;
    near_clipping_corners = frustum->_far_clipping_corners;

    first_hitpoints = dp->get_first_hits();
    second_hitpoints = dp->get_second_hits();
    sample_grid = dp->get_sample_grid();
    dome_vertices = dp->get_dome_vertices();

    // cleanup
    delete dp;
    delete mirror;
    delete dome;
}

/**
 * main function
 * @return
 */
int main() {

    // load config
    json11::Json config;
    if (loadConfig("../config/config.json", config)) {
        model_config = config["model"].object_items();
        application_config = config["application"].object_items();
    } else {
        return 0;
    }

    runModelCalculations();

    initializeGLContext();

    // generate vertex array
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    vertex_array_ids.push_back(vertex_array_id);

    // create buffers
    vertex_buffer_ids.push_back(createVertexBuffer(vertex_buffer_data::quad));

    setupBuffers();

    // load shaders
    GLuint program_id = LoadShaders("../shaders/simple.vert", "../shaders/simple.frag");
    shader_program_ids.push_back(program_id);

    // build model view projector_projection matrix
    // Get a handle for our "MVP" uniform
    GLint matrix_id = glGetUniformLocation(program_id, "MVP");

    origin.emplace_back((0, 0, 0));

    // create camera projection matrix
    float fov = (float)application_config["camera"]["fov"].number_value();
    int screen_width = (int)application_config["camera"]["screen"]["w"].number_value();
    int screen_height = (int)application_config["camera"]["screen"]["h"].number_value();
    glm::mat4 camera_projection = glm::perspective(glm::radians(fov),
                                                   float(screen_width) / float(screen_height),
                                                   0.1f,
                                                   100.0f);


    glm::vec3 camera_world_pos = jsonArray2Vec3(application_config["camera"]["position"]);
    glm::vec3 camera_lookat = jsonArray2Vec3(application_config["camera"]["lookat"]);
    glm::vec3 camera_up = jsonArray2Vec3(application_config["camera"]["up"]);
    glm::mat4 camera_view = glm::lookAt(
            camera_world_pos,           // camera pos world space
            camera_lookat,              // lookat
            camera_up                   // up vec
    );

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

