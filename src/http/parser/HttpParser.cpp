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

		// BodyState 또는 DoneState로 전환되면 본문 처리로 넘어간다.
		if (dynamic_cast<BodyState*>(_currentState) ||
			dynamic_cast<DoneState*>(_currentState))
			break;
	}

	// BodyState일 경우 Content-Length 만큼 그대로 읽어 전달
	BodyState* bodyState = dynamic_cast<BodyState*>(_currentState);
	if (bodyState) {
		size_t		contentLength = 0;
		std::string lengthStr = _packet->getHeader().get("Content-Length");
		if (!lengthStr.empty()) contentLength = std::stoi(lengthStr);

		std::string body(contentLength, '\0');
		stream.read(&body[0], contentLength);
		body.resize(stream.gcount());

		bodyState->parse(this, body);
		bodyState->handleNextState(this);
	}

	// 파싱 종료 후 DoneState에서도 한 번 호출돼 결과 설정
	_currentState->parse(this, std::string());
}
// 파싱 결과 반환: HttpPacket 복사
HttpPacket HttpParser::getResult() { return *_packet; }
