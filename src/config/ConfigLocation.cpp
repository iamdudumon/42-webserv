#include "Config.hpp"

void Config::setLocationRoot(const std::string& path, const std::string& root) {
    _location[path]._root = root;
}

// index setter
void Config::setLocationIndex(const std::string& path, const std::string& index) {
    _location[path]._index = index;
}

void Config::setLocationAllowMethod(const std::string& path, const std::vector<std::string>& methods) {
    _location[path]._allow_method = methods;
}

const std::string& Config::getLocationRoot(const std::string& path) const {
    std::map<std::string, ConfigLocation>::const_iterator it = _location.find(path);
    if (it == _location.end()) {
        throw ConfigException("Invalid path: " + path);
    }
    return it->second._root;
}

const std::string& Config::getLocationIndex(const std::string& path) const {
    auto it = _location.find(path);
    if (it == _location.end()) {
        throw ConfigException("Invalid path: " + path);
    }
    return it->second._index;
}

const std::vector<std::string>& Config::getLocationAllowMethod(const std::string& path) const {
    auto it = _location.find(path);
    if (it == _location.end()) {
        throw ConfigException("Invalid path: " + path);
    }
    return it->second._allow_method;
}
