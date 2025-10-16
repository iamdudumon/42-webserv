// Parser.hpp
#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>

#include "../model/Packet.hpp"
#include "./state/BodyState.hpp"
#include "./state/ChunkedBodyState.hpp"
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
			bool _complete;

			Parser(const Parser&);
			Parser& operator=(const Parser&);

			friend class PacketLineState;
			friend class HeaderState;
			friend class BodyState;
			friend class ChunkedBodyState;
			friend class DoneState;

		public:
			Parser();
			~Parser();

			void parse();
			Packet getResult() const;
			void changeState(ParseState*);
			void append(const std::string&);
			bool isComplete() const;
			std::string tail() const;

			std::string readLine();
			std::string readBytes(size_t n);
	};
}  // namespace http

#endif
