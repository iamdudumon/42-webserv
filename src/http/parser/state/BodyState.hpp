// BodyState.hpp
#ifndef BODYSTATE_HPP
#define BODYSTATE_HPP

#include <string>

#include "ParseState.hpp"

class BodyState : public ParseState {
	public:
		BodyState(size_t remain) : _remain(remain), _done(false) {}

		virtual void parse(HttpParser*, const std::string&);
		virtual void handleNextState(HttpParser*);

	private:
		bool   _done;
		size_t _remain;	 // Content‑Length 가정
};

#endif