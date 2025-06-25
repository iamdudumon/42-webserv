#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>

# include "HttpPacket.hpp"

class HttpResponse : public HttpPacket {
	private:
		int _statusCode;
		std::string _statusText; // htpp 스펙상 정의된

	public:
		HttpResponse(std::string header, std::string body, int statusCode) : HttpPacket(header, body) {
			_statusCode = statusCode;
			_statusText = "OK"; // 추후 상태 코드에 따라 변경 예정
		}
};

#endif