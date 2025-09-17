// FileReader.hpp
#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include "FileInfo.hpp"

class FileReader {
	public:
		FileReader() {}
		~FileReader() {}
		static FileInfo readFile(const char*);
};

#endif
