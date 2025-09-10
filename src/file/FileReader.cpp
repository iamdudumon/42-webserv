#include "FileReader.hpp"

FileInfo FileReader::readFile(const char* path) {
	FileInfo info;
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		info.error = FileInfo::NOT_FOUND;
		return info;
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	// 읽기 중 오류 감지
	if (file.fail() && !file.eof()) {
		info.error = FileInfo::READ_ERROR;
		return info;
	}
	info.content = oss.str();
	info.error = FileInfo::NONE;
	return info;
}
