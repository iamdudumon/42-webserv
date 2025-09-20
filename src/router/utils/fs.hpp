// fs.hpp
#ifndef ROUTER_UTILS_FS_HPP
#define ROUTER_UTILS_FS_HPP

#include <string>

namespace router {
	namespace utils {
		bool exists(const std::string& path);
		bool isDir(const std::string& path);
		std::string join(const std::string& base, const std::string& rel);
		bool safeUnder(const std::string& root, const std::string& path);
	}  // namespace utils
}  // namespace router

#endif
