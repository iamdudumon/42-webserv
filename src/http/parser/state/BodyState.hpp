// BodyState.hpp
#ifndef HTTP_PARSER_BODYSTATE_HPP
#define HTTP_PARSER_BODYSTATE_HPP

#include <string>

#include "../Parser.hpp"
#include "ParseState.hpp"

namespace http {
	class BodyState : public ParseState {
		public:
			BodyState(size_t remain) : _done(false), _remain(remain) {}

			virtual void parse(Parser*);
			virtual void handleNextState(Parser*);

		private:
			bool _done;
			size_t _remain;
	};
}  // namespace http

#endif
