// fs.hpp
#ifndef ROUTER_FS_UTILS_HPP
#define ROUTER_FS_UTILS_HPP

#include <string>

namespace RouterUtils {
	namespace fs {
		bool exists(const std::string& path);
		bool isDir(const std::string& path);
		// 양쪽 경로를 이어 붙이고 필요 시 슬래시를 보정
		std::string join(const std::string& base, const std::string& rel);
		// 루트 밖으로 나가지 않는지(문자열 기준) 확인
		bool safeUnder(const std::string& root, const std::string& path);
	}
}

#endif
