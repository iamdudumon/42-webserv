#include "file_utils.hpp"

#include <fstream>
#include <sstream>

FileInfo readFile(const char* path) {
	FileInfo info;
	if (path == NULL) {
		info.error = FileInfo::NOT_FOUND;
		return info;
	}
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		info.error = FileInfo::NOT_FOUND;
		return info;
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	if (file.fail() && !file.eof()) {
		info.error = FileInfo::READ_ERROR;
		return info;
	}
	info.content = oss.str();
	info.error = FileInfo::NONE;
	return info;
}
