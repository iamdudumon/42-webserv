// HeaderState.hpp
#ifndef HEADERSTATE_HPP
#define HEADERSTATE_HPP

#include <string>

#include "ParseState.hpp"

class HeaderState : public ParseState {
	public:
		HeaderState() : _done(false) {}

		virtual void parse(HttpParser*, const std::string&);
		virtual void handleNextState(HttpParser*);

	private:
		bool _done;
};

#endif