// uri.hpp
#ifndef ROUTER_URI_UTILS_HPP
#define ROUTER_URI_UTILS_HPP

#include <string>

namespace RouterUtils {
	namespace uri {
		// 퍼센트 디코딩 (유효하지 않은 인코딩은 그대로 둠)
		std::string percentDecode(const std::string& s);
		// 경로에서 쿼리(#) 제거 후 중복 슬래시 압축 및 . .. 세그먼트 제거
		std::string normalizePath(const std::string& path);
		// 요청 타겟에서 쿼리 이전의 경로만 추출
		std::string extractPath(const std::string& target);
	}
}

#endif
