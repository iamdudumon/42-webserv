#include "FileReader.hpp"

std::string FileReader::readFile(const char* path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (file.is_open()) {
		std::ostringstream oss;
		oss << file.rdbuf();
		return oss.str();
	}
	return "";
}