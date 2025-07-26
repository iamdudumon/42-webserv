// Body.hpp
#pragma once
#ifndef BODY_HPP
#define BODY_HPP

#include <cstdint>
#include <vector>

#include "../types/HttpEnums.hpp"

class Body {
	private:
		std::vector<uint8_t>	 _data;
		HTTP::ContentType::Value _type;

	public:
		Body();
		~Body();
		Body(const Body&);
		Body& operator=(const Body&);

		const std::vector<uint8_t>& getData() const;
		HTTP::ContentType::Value	getType() const;

		void append(const char*, size_t);
		void setType(HTTP::ContentType::Value);
};

#endif
