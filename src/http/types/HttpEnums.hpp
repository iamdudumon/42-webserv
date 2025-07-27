// HttpEnums.hpp
#pragma once
#ifndef HTTPENUMS_HPP
#define HTTPENUMS_HPP

#include <cstddef>
#include <string>

#include "../../utils/str_utils.hpp"

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

		inline const Value to_value(const std::string& str) {
			if (str == "GET") return GET;
			if (str == "POST") return POST;
			if (str == "DELETE")
				return DELETE;
			else
				return UNKNOWN_METHOD;
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
					return "200";
				case Created:
					return "201";
				case NoContent:
					return "204";
				case BadRequest:
					return "400";
				case Unauthorized:
					return "401";
				case Forbidden:
					return "403";
				case NotFound:
					return "404";
				case InternalServerError:
					return "500";
				default:
					return "0";
			}
		}

		inline const Value to_value(const std::string& str) {
			return Value(str_toint(str));
		}

		inline const char* to_reasonPhrase(Value v) {
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
			UNKNOWN_TYPE,
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

		inline const Value to_value(const std::string& str) {
			if (str == "text/plain") return CONTENT_TEXT_PLAIN;
			if (str == "text/html") return CONTENT_TEXT_HTML;
			if (str == "multipart/form-data") return CONTENT_MULTIPART_FORMDATA;
			if (str == "application/json") return CONTENT_APPLICATION_JSON;
			return UNKNOWN_TYPE;
		}
	}
}

#endif
