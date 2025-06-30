// Body.hpp
#pragma once
#ifndef BODY_HPP
#define BODY_HPP

#include <vector>

#include "../types/HttpEnums.hpp"

class Body {
	private:
		std::vector<uint8_t>	 _data;
		HTTP::ContentType::Value _type;

	public:
		std::vector<uint8_t>&	 getData() const;
		HTTP::ContentType::Value getType() const;
};

#endif
