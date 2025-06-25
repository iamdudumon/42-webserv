#ifndef HTTPPACKET_HPP
# define HTTPPACKET_HPP

# include <string>

class HttpPacket {
	protected:
		const std::string _version;
		std::string _header; // 구체화 예정
		std::string _body;

	public:
		HttpPacket(std::string header, std::string body): _version("HTTP/1.1"), _header(header), _body(body) {}

};

#endif