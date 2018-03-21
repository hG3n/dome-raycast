//
// Created by Hagen Hiller on 21/03/18.
//

#ifndef RAYCAST_PATH_HPP
#define RAYCAST_PATH_HPP

#include <string>
#include <vector>

class Path {

public:
    /**
     * Empty c'tor
     */
    Path();

    /**
     * Constructs a Path object containing an initial directory
     * @param path
     */
    Path(std::string const &path);

    /**
     * Constructs a Path object by passing std::vector of std::strings
     * @param path
     */
    Path(std::vector<std::string> const &path);


    /**
     * Pushes a single object to the end of the string
     * @param path
     */
    void push_s(std::string const &path);

    /**
     * Pushes multiple elements to the end of the string
     * from std::vector<std::string>>
     * @param path
     */
    void push_m(std::vector<std::string> const &path);

    /**
     * Removes the last paths element
     */
    void pop();

    /**
     * Returns std::string from path
     * @return std::string const&
     */
    std::string const &str() const;
private:
    std::vector<std::string> _path;

};

#endif //RAYCAST_PATH_HPP
