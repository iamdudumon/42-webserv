// mime.cpp
#include "mime.hpp"

#include <map>

namespace {
	std::map<std::string, std::string> initMimeMap() {
		std::map<std::string, std::string> m;
		m[".html"] = "text/html";
		m[".txt"] = "text/plain";
		m[".css"] = "text/css";
		m[".js"] = "application/javascript";
		m[".json"] = "application/json";
		m[".png"] = "image/png";
		m[".jpg"] = "image/jpeg";
		return m;
	}

	const std::map<std::string, std::string> g_mimeMap = initMimeMap();
}

namespace router {
	namespace utils {
		std::string byExtension(const std::string& filename) {
			size_t dot = filename.find_last_of('.');
			if (dot == std::string::npos) return "application/octet-stream";

			std::string ext = filename.substr(dot);
			std::map<std::string, std::string>::const_iterator it = g_mimeMap.find(ext);
			if (it != g_mimeMap.end()) return it->second;

			return "application/octet-stream";
		}
	}
}  // namespace router
