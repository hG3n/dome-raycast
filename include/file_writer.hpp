//
// Created by Hagen Hiller on 19/03/18.
//


#ifndef RAYCAST_FILE_WRITER_HPP
#define RAYCAST_FILE_WRITER_HPP

#include <string>
#include <vector>

#include <glm/vec3.hpp>

#include "path.hpp"

class FileWriter {

public:

    /**
     * c'tor
     */
    FileWriter();

    /**
     * c'tor
     * @param path
     */
    FileWriter(Path path);

    /**
     * Sets the path
     * @param path
     */
    void setPath(Path const &path);

    /**
     * Saves std::vector of glm::vec3 to given filename
     * @param point_list
     * @param file
     */
    void writeVec3List(std::vector<glm::vec3> const &point_list, std::string const &file);

private:
    Path _path;
};

#endif //RAYCAST_FILE_WRITER_HPP
