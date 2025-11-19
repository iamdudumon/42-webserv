#ifndef HTTP_RESPONSEFACTORY_HPP
#define HTTP_RESPONSEFACTORY_HPP

#include <string>

#include "../config/model/Config.hpp"
#include "Enums.hpp"
#include "model/Packet.hpp"

namespace http {
	class ResponseFactory {
		private:
			ResponseFactory();
			~ResponseFactory();

			static Packet create(StatusCode::Value, const std::string& = "", const std::string& = "");

		public:
			static Packet createFileResponse(StatusCode::Value, const std::string&,
											 const std::string& = "");
			static Packet createError(StatusCode::Value, const config::Config* = NULL,
									  const std::string& = "", const std::string& = "");
			static Packet createRedirect(StatusCode::Value, const std::string&);
			static std::string createCgiResponse(const std::string&, bool);
	};
}  // namespace http

#endif
