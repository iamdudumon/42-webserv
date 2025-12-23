// HaederState.cpp
#include "HeaderState.hpp"

#include <cctype>

#include "../../../utils/str_utils.hpp"
#include "../exception/NeedMoreInput.hpp"
#include "../exception/ParserException.hpp"
#include "BodyState.hpp"
#include "ChunkedBodyState.hpp"
#include "DoneState.hpp"

namespace http {
	void HeaderState::parse(Parser* parser) {
		if (_done) return;

		while (true) {
			std::string line;
			try {
				line = parser->readLine();
			} catch (const NeedMoreInput&) {
				if (parser->inputEnded())
					throw ParserException("Malformed request: header line truncated",
										  http::StatusCode::BadRequest);
				throw;
			}
			if (line.empty()) {
				_done = true;
				return;
			}

			size_t sep = line.find(':');
			if (sep == std::string::npos || sep == 0)
				throw ParserException("Malformed header line", http::StatusCode::BadRequest);

			std::string key = line.substr(0, sep);
			std::string value = line.substr(sep + 1);

			value.erase(0, value.find_first_not_of(" 	"));
			value.erase(value.find_last_not_of(" 	") + 1);

			parser->_packet->addHeader(key, value);
		}
	}

	void HeaderState::handleNextState(Parser* parser) {
		if (!_done) return;
		if (parser->_packet->isRequest() && parser->_packet->getHeader().get("host").empty())
			throw ParserException("Host header is missing", http::StatusCode::BadRequest);

		std::string transferEncoding = parser->_packet->getHeader().get("Transfer-Encoding");
		bool isChunked = false;
		if (!transferEncoding.empty()) {
			std::string normalized = to_lower(transferEncoding);
			if (normalized.find("chunked") != std::string::npos)
				isChunked = true;
			else
				throw ParserException("Unsupported Transfer-Encoding",
									  http::StatusCode::BadRequest);
		}

		std::string lengthStr = parser->_packet->getHeader().get("Content-Length");
		if (isChunked && !lengthStr.empty())
			throw ParserException("Content-Length must not be sent with chunked body",
								  http::StatusCode::BadRequest);

		if (isChunked) {
			parser->_packet->applyBodyLength(0);
			parser->changeState(new ChunkedBodyState());
			return;
		}

		size_t contentLength = 0;
		if (!lengthStr.empty()) {
			for (size_t i = 0; i < lengthStr.size(); ++i) {
				if (!isdigit(static_cast<unsigned char>(lengthStr[i])))
					throw ParserException("Invalid Content-Length value",
										  http::StatusCode::BadRequest);
			}
			contentLength = str_toint(lengthStr);
		}
		if (contentLength > parser->_maxBodySize)
			throw ParserException("Payload too large", http::StatusCode::RequestEntityTooLarge);

		http::ContentType::Value contentType =
			http::ContentType::to_value(parser->_packet->getHeader().get("Content-Type"));

		parser->_packet->applyBodyLength(contentLength);
		parser->_packet->applyBodyType(contentType);
		if (contentLength == 0)
			parser->changeState(new DoneState());
		else
			parser->changeState(new BodyState(contentLength));
	}
}  // namespace http
