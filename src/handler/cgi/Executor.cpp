// Executor.cpp
#include "Executor.hpp"

#include <sys/socket.h>

#include <cstdlib>

#include "../../utils/str_utils.hpp"

using namespace handler::cgi;

void Executor::setArgv(const router::RouteDecision& decision) {
	_argv.push_back(const_cast<char*>("/usr/bin/python3"));
	_argv.push_back(const_cast<char*>(decision.fsPath.c_str()));
	_argv.push_back(NULL);
}

void Executor::setEnvp(const router::RouteDecision& decision, const http::Packet& request) {
	std::string requestMethod = http::Method::to_string(request.getStartLine().method);
	std::string contentType = request.getHeader().get("Content-Type");
	std::string contentLength = request.getHeader().get("Content-Length");
	std::string scriptName = request.getStartLine().target;
	std::string serverPort = int_tostr(decision.server->getListen());
	std::string serverName = decision.server->getServerName();
	std::string serverProtocol = request.getStartLine().version;
	std::string uploadPath = decision.server->getUploadPath();

	_envStrings.push_back("REQUEST_METHOD=" + requestMethod);
	_envStrings.push_back("QUERY_STRING=" + decision.queryString);
	_envStrings.push_back("CONTENT_TYPE=" + contentType);
	_envStrings.push_back("CONTENT_LENGTH=" + contentLength);
	_envStrings.push_back("SCRIPT_NAME=" + scriptName);
	_envStrings.push_back("PATH_INFO=" + decision.fsPath);
	_envStrings.push_back("PATH_TRANSLATED=" + decision.fsRoot + decision.fsPath);
	_envStrings.push_back("SERVER_NAME=" + serverName);
	_envStrings.push_back("SERVER_PORT=" + serverPort);
	_envStrings.push_back("SERVER_PROTOCOL=" + serverProtocol);
	_envStrings.push_back("REMOTE_ADDR=127.0.0.1");
	_envStrings.push_back("REMOTE_PORT=0");
	_envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_envStrings.push_back("UPLOAD_PATH=" + uploadPath);

	for (size_t i = 0; i < _envStrings.size(); ++i) {
		_envp.push_back(const_cast<char*>(_envStrings[i].c_str()));
	}
	_envp.push_back(NULL);
}

void Executor::execute(const router::RouteDecision& decision, const http::Packet& request,
					   server::EpollManager& epollManager, cgi::ProcessManager& cgiManager,
					   int clientFd) {
	int stdoutPair[2];
	int stdinPair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, stdoutPair) == -1 ||
		socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, stdinPair) == -1) {
		throw Exception();
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(stdoutPair[0]);
		close(stdoutPair[1]);
		close(stdinPair[0]);
		close(stdinPair[1]);
		throw Exception();
	}

	if (pid == 0) {
		close(stdoutPair[0]);
		close(stdinPair[0]);
		if (dup2(stdoutPair[1], STDOUT_FILENO) == -1 || dup2(stdoutPair[1], STDERR_FILENO) == -1 ||
			dup2(stdinPair[1], STDIN_FILENO) == -1) {
			exit(1);
		}
		close(stdoutPair[1]);
		close(stdinPair[1]);

		setArgv(decision);
		setEnvp(decision, request);

		execve(_argv[0], _argv.data(), _envp.data());
		exit(1);
	}

	close(stdoutPair[1]);
	close(stdinPair[1]);

	std::string bodyPayload;
	if (request.getStartLine().method == http::Method::POST) {
		const std::vector<unsigned char>& bodyData = request.getBody().getData();
		if (!bodyData.empty())
			bodyPayload.assign(reinterpret_cast<const char*>(bodyData.data()), bodyData.size());
	}

	cgiManager.registerProcess(pid, stdoutPair[0], stdinPair[0], clientFd, bodyPayload,
							   epollManager);
}
