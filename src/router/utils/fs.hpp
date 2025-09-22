// fs.hpp
#ifndef ROUTER_UTILS_FS_HPP
#define ROUTER_UTILS_FS_HPP

#include <string>

namespace router {
	namespace utils {
		bool exists(const std::string&);
		bool isDir(const std::string&);
		std::string join(const std::string&, const std::string&);
		bool safeUnder(const std::string&, const std::string&);
	}  // namespace utils
}  // namespace router

#endif
