#ifndef HTTP_RESPONSE_FACTORY_HPP
#define HTTP_RESPONSE_FACTORY_HPP

#include <string>

#include "../../config/model/Config.hpp"
#include "../Enums.hpp"
#include "../model/Packet.hpp"

namespace http {
	namespace response {
		class Factory {
			private:
				Factory();
				~Factory();

			public:
				static Packet create(StatusCode::Value, const std::string& = "",
									 const std::string& = "");

				static Packet createFileResponse(StatusCode::Value, const std::string&,
												 const std::string& = "");
				static Packet createError(StatusCode::Value, const config::Config* = NULL,
										  const std::string& = "", const std::string& = "");
				static Packet createRedirect(StatusCode::Value, const std::string&);
		};
	}  // namespace response
}  // namespace http

#endif
