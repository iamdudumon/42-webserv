// HeaderState.hpp
#ifndef HTTP_PARSER_HEADERSTATE_HPP
#define HTTP_PARSER_HEADERSTATE_HPP

#include "../Parser.hpp"
#include "ParseState.hpp"

namespace http {
	class HeaderState : public ParseState {
		public:
			HeaderState() : _done(false) {}

			virtual void parse(Parser*);
			virtual void handleNextState(Parser*);

		private:
			bool _done;
	};

}  // namespace http

#endif
