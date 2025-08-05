#include <iostream>
#include <vector>

#include "./model/HttpPacket.hpp"
#include "./parser/HttpParser.hpp"
#include "./parser/exception/HttpParseException.hpp"
#include "./types/HttpEnums.hpp"

// 파싱 및 결과 출력을 위한 함수
void test_request(const std::string& request_data) {
	std::cout << "--- Input Request Data ---" << std::endl;
	std::cout << request_data << std::endl;
	std::cout << "--------------------------" << std::endl;

	try {
		// --- 파서 실행 ---
		HttpParser parser(request_data);
		parser.parse();
		HttpPacket result = parser.getResult();

		// --- 성공 결과 출력 ---
		std::cout << "--- Parsing Result (Success) ---" << std::endl;
		if (result.isRequest()) {
			HTTP::StartLine startLine = result.getStartLine();
			std::cout << "Method: " << HTTP::Method::to_string(startLine.method)
					  << std::endl;
			std::cout << "Target: " << startLine.target << std::endl;
			std::cout << "Version: " << startLine.version << std::endl;
		}
		std::cout << "Host Header: " << result.getHeader().get("host")
				  << std::endl;

	} catch (const HttpParseException& e) {
		// --- 예외 처리 ---
		// 서버는 이 정보를 바탕으로 실제 오류 응답(HTML 등)을 생성합니다.
		std::cerr << "--- Parsing Result (Exception Caught) ---" << std::endl;
		std::cerr << "Server would generate an error response with:"
				  << std::endl;
		std::cerr << "  Status Code: "
				  << HTTP::StatusCode::to_reasonPhrase(e.getStatusCode())
				  << std::endl;
		std::cerr << "  Message: " << e.what() << std::endl;
	}
	std::cout << "========================================" << std::endl
			  << std::endl;
}

int main() {
	// 1. 정상적인 요청 (Host 헤더 포함)
	std::string valid_request =
		"GET /index.html HTTP/1.1\r\n"
		"Host: www.example.com\r\n"
		"User-Agent: TestClient/1.0\r\n"
		"\r\n";
	test_request(valid_request);

	// 2. 비정상적인 요청 (Host 헤더 누락)
	std::string invalid_request_no_host =
		"GET /index.html HTTP/1.1\r\n"
		"User-Agent: TestClient/1.0\r\n"
		"\r\n";
	test_request(invalid_request_no_host);

	// 3. 비정상적인 요청 (CRLF 누락)
	std::string invalid_request_no_crlf =
		"GET /index.html HTTP/1.1\r\n"
		"Host: www.example.com";  // 마지막 CRLF가 없습니다.
	test_request(invalid_request_no_crlf);

	// 4. 비정상적인 요청 (Content-Length header 없이 body data)
	std::string invalid_request_no_content_length =
		"GET /index.html HTTP/1.1\r\n"
		"Host: www.example.com\r\n"
		"User-Agent: TestClient/1.0\r\n"
		"\r\n"
		"1\r\n"
		"1\r\n";
	test_request(invalid_request_no_content_length);

	return 0;
}