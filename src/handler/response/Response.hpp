// Response.hpp
#ifndef HANDLER_RESPONSE_HPP
#define HANDLER_RESPONSE_HPP

#include <cstring>
#include <string>

#include "../../config/model/Config.hpp"
#include "../../http/Enums.hpp"
#include "../../http/model/Packet.hpp"
#include "../../http/serializer/Serializer.hpp"
#include "../../utils/file_utils.hpp"
#include "../../utils/str_utils.hpp"
#include "../cgi/validator/Validator.hpp"
#include "../exception/Exception.hpp"

namespace handler {
	class response {
		private:
			response() {}
			~response() {}

		public:
			static http::Packet makePlainResponse(http::StatusCode::Value, const std::string&,
												  const std::string&);
			static http::Packet makeErrorResponse(
				http::StatusCode::Value status, const config::Config* config = NULL,
				const std::string& fallbackBody = std::string(),
				const std::string& fallbackContentType = std::string());
			static std::string makeCgiResponse(const std::string&);
	};

	namespace utils {
		inline bool loadPageContent(const std::string& path, std::string& out) {
			if (path.empty()) return false;
			FileInfo file = readFile(path.c_str());
			if (file.error != FileInfo::NONE) return false;
			out = file.content;
			return true;
		}
	}  // namespace utils
}  // namespace handler

#endif
