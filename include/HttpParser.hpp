#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpParser {
	private:

	public:
		static HttpRequest	parseRequestHttp(std::string rawDate) {
			std::string header;
			std::string body;
			std::string	httpMethod; // enum
			std::string target;
			// 파싱로직 추가
			HttpRequest httpRequest(header, body, httpMethod, target);
			return httpRequest;
		}

		static HttpResponse	parseResponseHttp(std::string rawDate){
			std::string	header;
			std::string	body;
			int			statusCode; // enum?
			// 파싱로직 추가
			HttpResponse httpResponse(header, body, statusCode);
			return httpResponse;
		}

};

#endif