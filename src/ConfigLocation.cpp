#include "../include/ConfigLocation.hpp"

void ConfigLocation::setRoot(const std::string& root) {
    _root = root;
}

// index setter
void ConfigLocation::setIndex(const std::string& index) {
    _index = index;
}

// limit_except 전체 교체 setter
void ConfigLocation::setLimitExcept(const std::vector<std::string>& methods) {
    _limit_except = methods;
}

const std::string& ConfigLocation::getRoot() const {
    return _root;
}

const std::string& ConfigLocation::getIndex() const {
    return _index;
}

const std::vector<std::string>& ConfigLocation::getLimitExcept() const {
    return _limit_except;
}
