#ifndef HANDLER_EVENTRESULT_HPP
#define HANDLER_EVENTRESULT_HPP

#include <string>

#include "../../http/model/Packet.hpp"

namespace handler {
	class EventResult {
		private:
			void copyFrom(const EventResult&);

		public:
			int fd;
			http::Packet* packet;
			std::string raw;
			bool useRaw;
			bool keepAlive;
			bool closeAfterSend;
			int closeFd;

			EventResult();
			EventResult(const EventResult&);
			EventResult& operator=(const EventResult&);
			~EventResult();

			void setPacket(const http::Packet&);
			void clearPacket();
			void setPacketResponse(int, const http::Packet&, bool);
			void setRawResponse(int, const std::string&, bool);
			void reset(int);
	};
}  // namespace handler

#endif
