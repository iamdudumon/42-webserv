// Serializer.hpp
#ifndef HTTP_SERIALIZER_SERIALIZER_HPP
#define HTTP_SERIALIZER_SERIALIZER_HPP

#include <string>

#include "../model/Packet.hpp"

namespace http {
	class Serializer {
		public:
			static std::string serialize(const Packet&);
	};
}  // namespace http

#endif
