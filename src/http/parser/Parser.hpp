// Parser.hpp
#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>

#include "../model/Packet.hpp"
#include "./state/BodyState.hpp"
#include "./state/DoneState.hpp"
#include "./state/HeaderState.hpp"
#include "./state/PacketLineState.hpp"
#include "./state/ParseState.hpp"

namespace http {
	class Parser {
		private:
			ParseState* _currentState;
			std::string _rawData;
			size_t _pos;
			Packet* _packet;

			friend class PacketLineState;
			friend class HeaderState;
			friend class BodyState;
			friend class DoneState;

		public:
			Parser(const std::string&);
			~Parser();

			void parse();
			Packet getResult();
			void changeState(ParseState*);

			std::string readLine();
			std::string readBytes(size_t n);
			size_t remaining() const;
	};
}  // namespace http

#endif
