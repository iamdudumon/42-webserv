// HttpParser.hpp
#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <string>

#include "../model/HttpPacket.hpp"
#include "./state/BodyState.hpp"
#include "./state/DoneState.hpp"
#include "./state/HeaderState.hpp"
#include "./state/PacketLineState.hpp"
#include "./state/ParseState.hpp"

class HttpParser {
	private:
		ParseState* _currentState;
		std::string _rawData;
		HttpPacket* _packet;

		friend class PacketLineState;
		friend class HeaderState;
		friend class BodyState;
		friend class DoneState;

	public:
		HttpParser(const std::string&);
		~HttpParser();

		void	   parse();
		HttpPacket getResult();
		void	   changeState(ParseState*);
};

#endif
