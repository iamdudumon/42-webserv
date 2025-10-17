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
			bool _inputEnded;

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

			Packet getResult() const;
			bool isComplete() const;
			bool inputEnded() const;
			std::string tail() const;
			void markEndOfInput();

			void parse();
			void append(const std::string&);
			void changeState(ParseState*);

			std::string readLine();
			std::string readBytes(size_t);
	};
}  // namespace http

#endif
