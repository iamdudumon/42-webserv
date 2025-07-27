// BodyState.cpp
#include "BodyState.hpp"

void BodyState::parse(HttpParser* parser, const std::string& line) {
        if (_remain == 0) {
                _done = true;
                return;
        }
        // 남은 본문 길이만큼 복사
        size_t toCopy = std::min(_remain, line.size());
        // HttpPacket에 본문 데이터 추가
        parser->_packet->appendBody(line.data(), toCopy);
        _remain -= toCopy;
        if (_remain == 0) _done = true;
}

void BodyState::handleNextState(HttpParser* parser) {
	if (_done) parser->changeState(new DoneState());
}
