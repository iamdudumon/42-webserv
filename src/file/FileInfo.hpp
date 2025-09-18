// FileInfo.hpp
#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <string>

struct FileInfo {
		std::string content;
		enum Error {
			NONE,
			NOT_FOUND,
			READ_ERROR
		} error;
};

#endif
