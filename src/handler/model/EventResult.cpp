#include "EventResult.hpp"

#include <cstddef>

using namespace handler;

void EventResult::copyFrom(const EventResult& other) {
	fd = other.fd;
	raw = other.raw;
	useRaw = other.useRaw;
	keepAlive = other.keepAlive;
	closeAfterSend = other.closeAfterSend;
	closeFd = other.closeFd;
	delete packet;
	packet = other.packet ? new http::Packet(*other.packet) : NULL;
}

EventResult::EventResult() :
	fd(-1),
	packet(NULL),
	raw(),
	useRaw(false),
	keepAlive(false),
	closeAfterSend(false),
	closeFd(-1) {}

EventResult::EventResult(const EventResult& other) : fd(-1), packet(NULL) {
	copyFrom(other);
}

EventResult& EventResult::operator=(const EventResult& other) {
	if (this != &other) copyFrom(other);
	return *this;
}

EventResult::~EventResult() {
	delete packet;
}

void EventResult::setPacket(const http::Packet& value) {
	delete packet;
	packet = new http::Packet(value);
}

void EventResult::clearPacket() {
	delete packet;
	packet = NULL;
}

void EventResult::setPacketResponse(int socketFd, const http::Packet& value, bool keep) {
	fd = socketFd;
	setPacket(value);
	raw.clear();
	useRaw = false;
	keepAlive = keep;
	closeAfterSend = !keep;
}

void EventResult::setRawResponse(int socketFd, const std::string& value, bool keep) {
	fd = socketFd;
	clearPacket();
	raw = value;
	useRaw = true;
	keepAlive = keep;
	closeAfterSend = !keep;
}

void EventResult::reset() {
	fd = -1;
	clearPacket();
	raw.clear();
	useRaw = false;
	keepAlive = false;
	closeAfterSend = false;
	closeFd = -1;
}
