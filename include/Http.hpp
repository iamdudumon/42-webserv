#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>

class Http {
	protected:
		const std::string _version;
		std::string _header; // 구체화 예정
		std::string _body;

	public:
		Http(std::string header, std::string body): _version("HTTP/1.1"), _header(header), _body(body) {}

};

#endif