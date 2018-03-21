//
// Created by Hagen Hiller on 19/03/18.
//

#include "file_writer.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

FileWriter::FileWriter()
        : _path() {}


FileWriter::FileWriter(Path path)
        : _path(path) {}


void FileWriter::setPath(Path const &path) {
    _path = path;
}


void FileWriter::writeVec3List(std::vector<glm::vec3> const &point_list, std::string const &file) {
    std::vector<glm::vec3> list_cpy(point_list);

    std::stringstream oss;
    for (auto point: list_cpy) {
        oss << point.x << " " << point.y << " " << point.z << std::endl;
    }

    _path.push_s(file);
    std::ofstream out_stream;
    out_stream.open(_path.str());
    out_stream << oss.str();
    out_stream.close();

    std::cout << "Successfully saved '" << file << "' to '" << _path.str() << "'." << std::endl;

}

