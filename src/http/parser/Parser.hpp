// Parser.hpp
#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <cstddef>
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
					http::Packet packet;
					std::string leftover;
					http::StatusCode::Value errorCode;
					std::string errorMessage;
					bool endOfInput;

					Result() :
						status(Incomplete),
						packet(http::StatusLine(), http::Header(), http::Body()),
						errorCode(http::StatusCode::BadRequest),
						errorMessage(
							http::StatusCode::to_reasonPhrase(http::StatusCode::BadRequest)),
						endOfInput(false) {}
			};

			Parser();
			~Parser();

			bool inputEnded() const;
			void markEndOfInput();
			size_t getMaxBodySize() const;
			void setMaxBodySize(size_t);

			Result parse();
			void append(const std::string&);
			void changeState(ParseState*);
			void reset();

			std::string readLine();
			std::string readBytes(size_t);
	};
}  // namespace http

#endif
