// uri.cpp
#include "uri.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace {
	static int hexVal(char c) {
		if ('0' <= c && c <= '9') return c - '0';
		if ('a' <= c && c <= 'f') return c - 'a' + 10;
		if ('A' <= c && c <= 'F') return c - 'A' + 10;
		return -1;
	}
}

namespace RouterUtils {
	namespace uri {
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

		static void pushSegment(std::vector<std::string>& segs, const std::string& s) {
			if (s.empty() || s == ".") return;
			if (s == "..") {
				if (!segs.empty()) segs.pop_back();
				return;
			}
			segs.push_back(s);
		}

		std::string normalizePath(const std::string& path) {
			// 쿼리/프래그먼트 제거는 호출 전 extract_path에서 처리
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
			// 프래그먼트는 일반적으로 요청 타겟에 포함되지 않지만 방어적으로 제거
			size_t hash = path.find('#');
			if (hash != std::string::npos) path = path.substr(0, hash);
			return path.empty() ? std::string("/") : path;
		}

	}
}