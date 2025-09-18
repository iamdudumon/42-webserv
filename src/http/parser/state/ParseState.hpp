// ParserState.hpp
#ifndef HTTP_PARSER_PARSESTATE_HPP
#define HTTP_PARSER_PARSESTATE_HPP

namespace http {
	class Parser;

	class ParseState {
		public:
			virtual ~ParseState() {};
			virtual void parse(Parser*) = 0;
			virtual void handleNextState(Parser*) = 0;
	};
}  // namespace http

#endif
