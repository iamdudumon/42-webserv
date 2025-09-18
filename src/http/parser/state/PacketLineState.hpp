// PacketLineState.hpp
#ifndef HTTP_PARSER_PACKETLINESTATE_HPP
#define HTTP_PARSER_PACKETLINESTATE_HPP

#include "../Parser.hpp"
#include "ParseState.hpp"

namespace http {
	class PacketLineState : public ParseState {
		public:
			PacketLineState() : _done(false) {}

			virtual void parse(Parser*);
			virtual void handleNextState(Parser*);

		private:
			bool _done;
	};
}  // namespace http

#endif
