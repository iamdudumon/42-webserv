// Parser.hpp
#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <cstddef>
#include <string>

#include "../model/Packet.hpp"
#include "./state/ParseState.hpp"

namespace {
	class BodyState;
	class ChunkedBodyState;
	class DoneState;
	class HeaderState;
	class PacketLineState;
}

namespace http {
	class Parser {
		private:
			ParseState* _currentState;
			std::string _rawData;
			size_t _pos;
			Packet* _packet;
			bool _complete;
			bool _inputEnded;
			size_t _maxBodySize;

			Parser(const Parser&);
			Parser& operator=(const Parser&);

			friend class PacketLineState;
			friend class HeaderState;
			friend class BodyState;
			friend class ChunkedBodyState;
			friend class DoneState;

		public:
			struct Result {
					enum Status {
						Incomplete,
						Completed,
						Error
					} status;
					http::Packet* packet;
					std::string leftover;
					http::StatusCode::Value errorCode;
					std::string errorMessage;
					bool endOfInput;

					Result() :
						status(Incomplete),
						packet(NULL),
						errorCode(http::StatusCode::BadRequest),
						errorMessage(
							http::StatusCode::to_reasonPhrase(http::StatusCode::BadRequest)),
						endOfInput(false) {}
			};

			Parser();
			~Parser();

			bool inputEnded() const;
			void markEndOfInput();
			void setMaxBodySize(size_t);
			bool hasUnconsumedInput() const;

			Result parse();
			void append(const std::string&);
			void changeState(ParseState*);
			void reset();

			std::string readLine();
			std::string readBytes(size_t);
	};
}  // namespace http

#endif
