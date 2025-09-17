// DoneState.hpp
#ifndef HTTP_PARSER_STATE_DONESTATE_HPP
#define HTTP_PARSER_STATE_DONESTATE_HPP

#include <string>

#include "../Parser.hpp"
#include "ParseState.hpp"

namespace http {
	class DoneState : public ParseState {
		public:
			virtual void parse(Parser*);
			virtual void handleNextState(Parser*);
	};
}  // namespace http

#endif
