// HttpEnums.hpp
#pragma once
#ifndef HTTPENUMS_HPP
#define HTTPENUMS_HPP

#include <cstddef>
#include <string>

namespace HTTP {
	namespace Method {
		enum Value {
			UNKNOWN_METHOD,
			GET,
			POST,
			DELETE,
		};

		inline const char* to_string(Value v) {
			switch (v) {
				case GET:
					return "GET";
				case POST:
					return "POST";
				case DELETE:
					return "DELETE";
				default:
					return "UNKNOWN_METHOD";
			}
		}
	}

	namespace StatusCode {
		enum Value {
			UnknownStatus = 0,
			OK = 200,
			Created = 201,
			NoContent = 204,
			BadRequest = 400,
			Unauthorized = 401,
			Forbidden = 403,
			NotFound = 404,
			InternalServerError = 500,
		};

		inline const char* to_string(Value v) {
			switch (v) {
				case OK:
					return "OK";
				case Created:
					return "Created";
				case NoContent:
					return "No Content";
				case BadRequest:
					return "Bad Request";
				case Unauthorized:
					return "Unauthorized";
				case Forbidden:
					return "Forbidden";
				case NotFound:
					return "Not Found";
				case InternalServerError:
					return "Internal Server Error";
				default:
					return "Unknown Status";
			}
		}
	}

	namespace ContentType {
		enum Value {
			UNKNOWN,
			CONTENT_TEXT_PLAIN,
			CONTENT_TEXT_HTML,
			CONTENT_MULTIPART_FORMDATA,
			CONTENT_APPLICATION_JSON,
		};

		inline const char* to_string(Value v) {
			switch (v) {
				case CONTENT_TEXT_PLAIN:
					return "text/plain";
				case CONTENT_TEXT_HTML:
					return "text/html";
				case CONTENT_MULTIPART_FORMDATA:
					return "multipart/form-data";
				case CONTENT_APPLICATION_JSON:
					return "application/json";
				default:
					return "unknown-type";
			}
		}
	}
}

#endif
