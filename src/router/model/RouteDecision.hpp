// RouteDecision.hpp
#ifndef ROUTE_DECISION_HPP
#define ROUTE_DECISION_HPP

#include <string>
#include <vector>

#include "../../config/model/Config.hpp"
#include "../../http/types/HttpEnums.hpp"

struct RouteDecision {
		enum Action {
			ServeFile,
			ServeAutoIndex,
			Cgi,
			Redirect,
			Error
		} action;

		// 공통 필드
		HTTP::StatusCode::Value status;	 // 정상 200, 에러/리다이렉트는 해당 코드
		const Config* server;			 // 선택된 서버
		std::string location_path;		 // 매칭된 location 접두사

		// 파일/디렉토리 관련
		std::string fs_root;			// 실제 루트 경로
		std::string fs_path;			// 최종 리소스 경로(파일/디렉토리)
		std::string index_used;			// 사용된 인덱스 파일명(있다면)
		std::string content_type_hint;	// 확장자 기반 MIME 힌트

		// 메서드 제한(405)용
		std::vector<std::string> allow_methods;	 // 405에 필요

		// 리다이렉트용
		std::string redirect_location;	// Location 헤더 값

		RouteDecision() :
			action(Error), status(HTTP::StatusCode::InternalServerError), server(NULL) {}
};

#endif
