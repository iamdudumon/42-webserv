// HttpParser.cpp
#include "HttpParser.hpp"

#include <sstream>

HttpParser::HttpParser(const std::string& rawData)
	: _currentState(new PacketLineState()), _rawData(rawData), _packet(NULL) {}

HttpParser::~HttpParser() {
	delete _currentState;
	if (_packet) delete _packet;
}

void HttpParser::changeState(ParseState* newState) {
	if (_currentState) {
		delete _currentState;
	}
	_currentState = newState;
}

// 파싱 실행: 줄 단위로 읽어 각 State에 전달

void HttpParser::parse() {
	std::istringstream stream(_rawData);
	std::string		   line;

	// 패킷 라인과 헤더를 우선 파싱한다.
	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();

		_currentState->parse(this, line);
		_currentState->handleNextState(this);

		if (dynamic_cast<DoneState*>(_currentState)) break;
	}

	// 파싱 종료 후 DoneState에서도 한 번 호출돼 결과 설정
	_currentState->parse(this, std::string());
}
// 파싱 결과 반환: HttpPacket 복사
HttpPacket HttpParser::getResult() { return *_packet; }
