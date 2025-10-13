// uri.cpp
#include "uri.hpp"

#include <string>
#include <vector>

namespace router {
	namespace utils {
		namespace {
			int hexVal(char c) {
				if ('0' <= c && c <= '9') return c - '0';
				if ('a' <= c && c <= 'f') return c - 'a' + 10;
				if ('A' <= c && c <= 'F') return c - 'A' + 10;
				return -1;
			}

			void pushSegment(std::vector<std::string>& segs, const std::string& s) {
				if (s.empty() || s == ".") return;
				if (s == "..") {
					if (!segs.empty()) segs.pop_back();
					return;
				}
				segs.push_back(s);
			}
		}  // unnamed namespace

		std::string percentDecode(const std::string& s) {
			std::string out;
			out.reserve(s.size());
			for (size_t i = 0; i < s.size(); ++i) {
				if (s[i] == '%' && i + 2 < s.size()) {
					int h1 = hexVal(s[i + 1]);
					int h2 = hexVal(s[i + 2]);
					if (h1 >= 0 && h2 >= 0) {
						out.push_back(static_cast<char>((h1 << 4) | h2));
						i += 2;
						continue;
					}
				}
				out.push_back(s[i]);
			}
			return out;
		}

		std::string normalizePath(const std::string& path) {
			std::string decoded = percentDecode(path);
			std::vector<std::string> segs;
			std::string cur;
			for (size_t i = 0; i < decoded.size(); ++i) {
				char c = decoded[i];
				if (c == '/') {
					pushSegment(segs, cur);
					cur.clear();
				} else {
					cur.push_back(c);
				}
			}
			pushSegment(segs, cur);

			std::string out = "/";
			for (size_t i = 0; i < segs.size(); ++i) {
				if (i) out += "/";
				out += segs[i];
			}
			return out;
		}

		std::string extractPath(const std::string& target) {
			size_t q = target.find('?');
			std::string path = (q == std::string::npos) ? target : target.substr(0, q);
			size_t hash = path.find('#');
			if (hash != std::string::npos) path.resize(hash);
			return path.empty() ? std::string("/") : path;
		}
	}  // namespace utils
}  // namespace router
