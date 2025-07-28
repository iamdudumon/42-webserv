// HttpParser.hpp
#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <string>

#include "../model/HttpPacket.hpp"
#include "./state/BodyState.hpp"
#include "./state/DoneState.hpp"
#include "./state/HeaderState.hpp"
#include "./state/PacketLineState.hpp"
#include "./state/ParseState.hpp"

class HttpParser {
	private:
		ParseState* _currentState;
		std::string _rawData;
		HttpPacket* _packet;

		// 상태 클래스에서 접근 허용
		friend class PacketLineState;
		friend class HeaderState;
		friend class BodyState;
		friend class DoneState;

	public:
		HttpParser(const std::string&);
		~HttpParser();

		void	   parse();					  // 상태 기반 파싱 시작
		HttpPacket getResult();				  // 최종 결과 반환
		void	   changeState(ParseState*);  // 상태 전환 메서드
};

#endif
