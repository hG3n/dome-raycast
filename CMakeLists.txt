cmake_minimum_required(VERSION 3.8)
project(raycast)

# compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused-parameter")

# find packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLM REQUIRED glm)

# try to find glew via cmake; if not found load via pkgconfig
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
endif ()

# use pkg-config to find further libs
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLM REQUIRED glm)

# set source files
set(SOURCE_FILES
        main.cpp
        sources/Sphere.cpp
        sources/Ray.cpp
        sources/Hitpoint.cpp
        sources/ShaderUtil.cpp
        sources/Utility.cpp)

# set header files
set(HEADER_FILES
        include/Sphere.hpp
        include/Ray.hpp
        include/Hitpoint.hpp
        include/ShaderUtil.hpp
        include/Utility.hpp)

# libraries
set(ALL_LIBS
        ${OPENGL_LIBRARIES}
        ${GLFW_STATIC_LIBRARIES}
        ${GLEW_LIBRARIES})

# specify executable
add_executable(raycast ${SOURCE_FILES} ${HEADER_FILES})

# get include directories
include_directories(
        include
        ${GLM_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INLCUDE_DIRS}
)

# link that stuff
target_link_libraries(raycast ${ALL_LIBS})
