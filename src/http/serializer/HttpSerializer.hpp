// HttpSerializer.hpp
#ifndef HTTPSERIALIZER_HPP
#define HTTPSERIALIZER_HPP

#include <string>

#include "../model/HttpPacket.hpp"

class HttpSerializer {
	public:
		static std::string serialize(const HttpPacket& packet);
};

#endif
