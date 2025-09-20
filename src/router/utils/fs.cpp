// fs.cpp
#include "fs.hpp"

#include <sys/stat.h>

namespace router {
	namespace utils {
		namespace {
			bool endsWithSlash(const std::string& s) {
				return !s.empty() && s[s.size() - 1] == '/';
			}

			bool startsWithSlash(const std::string& s) {
				return !s.empty() && s[0] == '/';
			}
		}

		bool exists(const std::string& path) {
			struct stat st;
			return stat(path.c_str(), &st) == 0;
		}

		bool isDir(const std::string& path) {
			struct stat st;
			if (stat(path.c_str(), &st) != 0) return false;
			return S_ISDIR(st.st_mode);
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
			if (path.size() > root.size()) {
				char c = path[root.size()];
				if (c != '/') return false;
			}
			return true;
		}
	}  // namespace utils
}  // namespace router
