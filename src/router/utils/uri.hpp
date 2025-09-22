// uri.hpp
#ifndef ROUTER_UTILS_URI_HPP
#define ROUTER_UTILS_URI_HPP

#include <string>

namespace router {
	namespace utils {
		std::string percentDecode(const std::string&);
		std::string normalizePath(const std::string&);
		std::string extractPath(const std::string&);
	}  // namespace utils
}  // namespace router

#endif
