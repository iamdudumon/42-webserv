// DoneState.cpp
#include "DoneState.hpp"

// parse: 파싱 완료 상태를 기록
void DoneState::parse(HttpParser*, const std::string&) {}

// handleNextState: 더 이상 전이할 상태가 없으므로 no-op
void DoneState::handleNextState(HttpParser* parser) {}
