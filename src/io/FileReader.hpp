#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <sstream>
#include <fstream>

#include "exception/IoException.hpp"

class FileReader {
	public:
		FileReader() { }
		~FileReader() { }
		static std::string readFile(const char*);
};

#endif