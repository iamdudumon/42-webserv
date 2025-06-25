#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

#include "Http.hpp"

class HttpRequest : public Http {
	private:
		std::string _httpMethod;
		std::string _target;

	public:
		HttpRequest(std::string header, std::string body, std::string httpMethod, std::string target):Http(header, body){
			_httpMethod = httpMethod;
			_target = target;
		}
};

#endif