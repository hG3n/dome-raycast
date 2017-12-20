cmake_minimum_required(VERSION 3.8)
project(raycast)

# compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused-parameter")

# find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLM REQUIRED glm)

# set source files
set(SOURCE_FILES
        main.cpp
        sources/Sphere.cpp
        sources/Ray.cpp
        sources/Hitpoint.cpp)

# set header files
set(HEADER_FILES
        include/Sphere.hpp
        include/Ray.hpp
        include/Hitpoint.hpp)

# specify executable
add_executable(raycast ${SOURCE_FILES} ${HEADER_FILES})

# get include directories
include_directories(
        include
        ${GLM_INCLUDE_DIRS}
)

# link that stuff
target_link_libraries(raycast ${ALL_LIBS})
