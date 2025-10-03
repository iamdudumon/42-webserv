// CgiBuilder.cpp
#include "Builder.hpp"

#include "../../utils/str_utils.hpp"

using namespace handler::cgi;

// 정적 멤버 정의
// std::map<int, cgi::Process> cgi::Builder::_active_processes;

void Builder::setArgv(const router::RouteDecision& decision) {
	_argv.clear();
	_argv.push_back(const_cast<char*>("/usr/bin/python3"));
	_argv.push_back(const_cast<char*>(decision.fs_path.c_str()));
	_argv.push_back(NULL);
}

void Builder::setEnvp(const router::RouteDecision& decision, const http::Packet& request) {
	std::string request_method = http::Method::to_string(request.getStartLine().method);
	std::string query_string = "";
	size_t query_pos = request.getStartLine().target.find('?');
	if (query_pos != std::string::npos) {
		query_string = request.getStartLine().target.substr(query_pos + 1);
	}
	std::string content_type = request.getHeader().get("Content-Type");
	std::string content_length = request.getHeader().get("Content-Length");

	std::string script_name = request.getStartLine().target;
	std::string server_port = int_tostr(decision.server->getListen());
	std::string server_name = decision.server->getServerName();

	// CGI 환경 변수 설정
	_env_strings.push_back("REQUEST_METHOD=" + request_method);
	_env_strings.push_back("QUERY_STRING=" + query_string);
	_env_strings.push_back("CONTENT_TYPE=" + content_type);
	_env_strings.push_back("CONTENT_LENGTH=" + content_length);
	_env_strings.push_back("SCRIPT_NAME=" + script_name);
	_env_strings.push_back("PATH_INFO=" + decision.fs_path);
	_env_strings.push_back("PATH_TRANSLATED=" + decision.fs_root + decision.fs_path);
	_env_strings.push_back("SERVER_NAME=" + server_name);
	_env_strings.push_back("SERVER_PORT=" + server_port);
	_env_strings.push_back("SERVER_PROTOCOL=" + request.getStartLine().version);
	_env_strings.push_back("REMOTE_ADDR=127.0.0.1");  // 로컬 테스트용 기본값
	_env_strings.push_back("REMOTE_PORT=0");		  // 기본값
	_env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
	for (size_t i = 0; i < _env_strings.size(); ++i) {
		_envp.push_back(const_cast<char*>(_env_strings[i].c_str()));
	}
	_envp.push_back(NULL);
}

void Builder::build(const router::RouteDecision& decision, const http::Packet& request,
					server::EpollManager& epoll_manager, cgi::Manager& cgi_manager, int client_fd) {
	// CGI 통신용 파이프 생성 (stdout은 논블로킹 모드로)
	int stdout_pipe[2], stdin_pipe[2];
	if (pipe2(stdout_pipe, O_NONBLOCK) == -1 || pipe(stdin_pipe) == -1) {
		throw Exception();
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		throw Exception();
	}

	if (pid == 0) {
		close(stdout_pipe[0]);
		close(stdin_pipe[1]);
		dup2(stdout_pipe[1], STDOUT_FILENO);
		dup2(stdout_pipe[1], STDERR_FILENO);
		close(stdout_pipe[1]);
		dup2(stdin_pipe[0], STDIN_FILENO);
		close(stdin_pipe[0]);

		setArgv(decision);
		setEnvp(decision, request);

		execve(_argv[0], _argv.data(), _envp.data());
		exit(1);
	}
	close(stdout_pipe[1]);
	close(stdin_pipe[0]);

	// 필요시 POST 데이터를 CGI로 전송
	if (request.getStartLine().method == http::Method::POST) {
		const std::vector<unsigned char>& body_data = request.getBody().getData();
		if (!body_data.empty()) {
			write(stdin_pipe[1], body_data.data(), body_data.size());
		}
	}
	close(stdin_pipe[1]);

	// 모니터링을 위해 CGI stdout을 epoll에 추가
	epoll_manager.add(stdout_pipe[0], EPOLLIN | EPOLLET);
	cgi_manager.registerProcess(pid, stdout_pipe[0], client_fd);
}
