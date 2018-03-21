#include "path.hpp"

Path::Path()
        : _path() {}

Path::Path(std::string const &path)
        : _path() {
    _path.push_back(path);
}

Path::Path(std::vector<std::string> const &path)
        : _path() {
    for (auto e : path) {
        _path.push_back(e);
    }
}

// - methods -
// -----------
void Path::push_s(std::string const &route) {
    _path.push_back(route);
}

void Path::push_m(std::vector<std::string> const &sub_path) {
    for (auto e: sub_path) {
        _path.push_back(e);
    }
}

void Path::pop() {
    _path.pop_back();
}

std::string const &Path::str() const {
    std::string res("");
    for (auto e: _path) {
        res += e + "/";
    }
    res.pop_back();
    return res;
}

