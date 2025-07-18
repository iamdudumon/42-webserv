// ParserState.hpp
#ifndef PARSESTATE_HPP
#define PARSESTATE_HPP

#include <string>

#include "HttpParser.hpp"

// 추상 상태 클래스
class ParseState {
	public:
		virtual ~ParseState() {};
		virtual void parse(HttpParser*, const std::string&) = 0;
		virtual void handleNextState(HttpParser*) = 0;
};

#endif
