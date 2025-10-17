// file_utils.hpp
#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <string>

struct FileInfo {
		std::string content;
		enum Error {
			NONE,
			NOT_FOUND,
			READ_ERROR
		} error;
};

FileInfo readFile(const char*);

#endif
