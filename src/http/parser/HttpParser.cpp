// HttpParser.cpp
#include "HttpParser.hpp"

#include <sstream>

// 생성자: 초기 상태를 PacketLineState로 설정
HttpParser::HttpParser(const std::string& rawData)
	: _currentState(new PacketLineState()), _rawData(rawData) {}

// 소멸자: 남아있는 상태 객체 해제
HttpParser::~HttpParser() { delete _currentState; }

// 상태 전환: 기존 상태 해제 후 교체
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

	// CRLF 분리를 위해 getline 사용 ('' 제거)
	while (std::getline(stream, line)) {
		// Remove trailing '\r'
		if (!line.empty() && line.back() == '\r') {
			line.erase(line.size() - 1);
		}
		// 현재 상태에 파싱 위임
		_currentState->parse(this, line);
		// 다음 상태로 전이
		_currentState->handleNextState(this);

		// Done 상태 도달 시 중지
		// DoneState의 handleNextState는 상태 변경 안 함
		// State 클래스 내부에서 DoneState로 전환된 후, DoneState를 검사
		// 여기서는 isRequest/isResponse 완료 여부 확인 가능
	}
	// 파싱 종료 후 DoneState에서도 한 번 호출돼 결과 설정
	_currentState->parse(this, std::string());
}

// 파싱 결과 반환: HttpPacket 복사
HttpPacket HttpParser::getResult() { return _packet; }
