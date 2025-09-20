// uri.hpp
#ifndef ROUTER_UTILS_URI_HPP
#define ROUTER_UTILS_URI_HPP

#include <string>

namespace router {
	namespace utils {
		std::string percentDecode(const std::string& s);
		std::string normalizePath(const std::string& path);
		std::string extractPath(const std::string& target);
	}  // namespace utils
}  // namespace router

#endif
