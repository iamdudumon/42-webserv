// Serializer.hpp
#ifndef HTTP_RESPONSE_SERIALIZER_HPP
#define HTTP_RESPONSE_SERIALIZER_HPP

#include <string>

#include "../model/Packet.hpp"

namespace http {
	namespace response {
		class Serializer {
			public:
				static std::string serialize(const Packet&, bool);
		};
	}  // namespace response
}  // namespace http

#endif
