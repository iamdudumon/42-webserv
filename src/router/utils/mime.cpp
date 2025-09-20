// mime.cpp
#include "mime.hpp"

#include <map>

namespace router {
	namespace utils {
		std::string byExtension(const std::string& filename) {
			static const std::map<std::string, std::string> kMap = {
				{".html", "text/html"},		   {".txt", "text/plain"},
				{".css", "text/css"},		   {".js", "application/javascript"},
				{".json", "application/json"}, {".png", "image/png"},
				{".jpg", "image/jpeg"},
			};
			size_t dot = filename.find_last_of('.');
			if (dot == std::string::npos) return "application/octet-stream";

			std::string ext = filename.substr(dot);
			std::map<std::string, std::string>::const_iterator it = kMap.find(ext);
			if (it != kMap.end()) return it->second;

			return "application/octet-stream";
		}
	}  // namespace utils
}  // namespace router
