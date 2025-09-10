// fs.cpp
#include "fs.hpp"

#include <sys/stat.h>

namespace RouterUtils {
	namespace fs {
		bool exists(const std::string& path) {
			struct stat st;
			return stat(path.c_str(), &st) == 0;
		}

		bool isDir(const std::string& path) {
			struct stat st;
			if (stat(path.c_str(), &st) != 0) return false;
			return S_ISDIR(st.st_mode);
		}

		static bool endsWithSlash(const std::string& s) {
			return !s.empty() && s[s.size() - 1] == '/';
		}

		static bool startsWithSlash(const std::string& s) {
			return !s.empty() && s[0] == '/';
		}

		std::string join(const std::string& base, const std::string& rel) {
			if (base.empty()) return rel;
			if (rel.empty()) return base;
			if (endsWithSlash(base) && startsWithSlash(rel)) return base + rel.substr(1);
			if (!endsWithSlash(base) && !startsWithSlash(rel)) return base + "/" + rel;
			return base + rel;
		}

		bool safeUnder(const std::string& root, const std::string& path) {
			if (root.empty()) return false;
			if (path.size() < root.size()) return false;
			if (path.compare(0, root.size(), root) != 0) return false;
			// 경계가 디렉터리 경계인지 확인 (루트가 "/var/www"이고 path가 "/var/www2"인 경우 방지)
			if (path.size() > root.size()) {
				char c = path[root.size()];
				if (c != '/') return false;
			}
			return true;
		}

	}
}
