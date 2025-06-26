#ifndef HTTPPARSER_HPP
# define HTTPPARSER_HPP

# include <string>
# include <iostream>

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class HttpParser {
	public:
		static HttpRequest	parseRequestHttp(std::string rawDate) {
			std::string header = "header";
			std::string body = "body";
			std::string	httpMethod = "httpMethod"; // enum
			std::string target = "target";
			// 파싱로직 추가
			std::cout << rawDate << std::endl;
			HttpRequest httpRequest(header, body, httpMethod, target);
			return httpRequest;
		}

		static HttpResponse	parseResponseHttp(std::string rawDate){
			std::string	header = "header";
			std::string	body = "body";
			int			statusCode = 200; // enum?
			// 파싱로직 추가
			std::cout << rawDate << std::endl;
			HttpResponse httpResponse(header, body, statusCode);
			return httpResponse;
		}

};

#endif