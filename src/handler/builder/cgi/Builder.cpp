// CgiBuilder.cpp
#include "Builder.hpp"

#include "../../../utils/str_utils.hpp"

using namespace handler::builder;

// Static member definition
std::map<int, cgi::Process> cgi::Builder::_active_processes;

void cgi::Builder::setArgv(const router::RouteDecision& decision) {
	_argv.clear();
	_argv.push_back(const_cast<char*>("/usr/bin/python3"));
	_argv.push_back(const_cast<char*>(decision.fs_path.c_str()));
	_argv.push_back(NULL);
}

void cgi::Builder::setEnvp(const router::RouteDecision& decision, const http::Packet& request,
						   const std::vector<config::Config>& configs) {
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

	// Set CGI environment variables
	_env_strings.push_back("REQUEST_METHOD=" + request_method);
	_env_strings.push_back("QUERY_STRING=" + query_string);
	_env_strings.push_back("CONTENT_TYPE=" + content_type);
	_env_strings.push_back("CONTENT_LENGTH=" + content_length);
	_env_strings.push_back("SCRIPT_NAME=" + script_name);
	_env_strings.push_back("PATH_INFO=" + decision.fs_path);
	_env_strings.push_back("PATH_TRANSLATED=" + decision.fs_root + decision.fs_path);
	_env_strings.push_back("SERVER_NAME=" + server_name);
	_env_strings.push_back("SERVER_PORT=" + server_port);
	//_env_strings.push_back("SERVER_PROTOCOL=" + request.getStartLine().version);
	//_env_strings.push_back("REMOTE_ADDR=127.0.0.1");  // Default for local testing
	//_env_strings.push_back("REMOTE_PORT=0");  // Default
	//_env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");

	for (size_t i = 0; i < _env_strings.size(); ++i) {
		_envp.push_back(const_cast<char*>(_env_strings[i].c_str()));
	}
	_envp.push_back(NULL);
}

std::string cgi::Builder::readPipe(int pipefd) {
	int n;
	char buf[4096];
	std::string output;
	while ((n = read(pipefd, buf, sizeof(buf))) > 0) {
		output.append(buf, n);
	}
	return output;
}

http::Packet cgi::Builder::build(const router::RouteDecision& decision, const http::Packet& request,
								 const std::vector<config::Config>& configs,
								 server::EpollManager& epoll_manager) {
	// Create pipes for CGI communication
	int stdout_pipe[2], stdin_pipe[2];
	if (pipe(stdout_pipe) == -1 || pipe(stdin_pipe) == -1) {
		throw Exception();
	}

	// Set non-blocking mode for reading from CGI
	int flags = fcntl(stdout_pipe[0], F_GETFL, 0);
	if (flags == -1 || fcntl(stdout_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1) {
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		throw Exception();
	}

	// Setup environment and arguments
	setArgv(decision);
	setEnvp(decision, request, configs);

	pid_t pid = fork();
	if (pid == -1) {
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		close(stdin_pipe[0]);
		close(stdin_pipe[1]);
		throw Exception();
	}

	if (pid == 0) {
		// Child process: execute CGI script
		close(stdout_pipe[0]);	// Close read end of stdout pipe
		close(stdin_pipe[1]);	// Close write end of stdin pipe

		// Redirect stdout to pipe
		dup2(stdout_pipe[1], STDOUT_FILENO);
		close(stdout_pipe[1]);

		// Redirect stdin from pipe (for POST data)
		dup2(stdin_pipe[0], STDIN_FILENO);
		close(stdin_pipe[0]);

		// Execute CGI script
		execve(_argv[0], _argv.data(), _envp.data());
		exit(1);  // execve failed
	}

	// Parent process: setup async monitoring
	close(stdout_pipe[1]);	// Close write end of stdout pipe
	close(stdin_pipe[0]);	// Close read end of stdin pipe

	// Write POST data to CGI if needed
	if (request.getStartLine().method == http::Method::POST) {
		const std::vector<unsigned char>& body_data = request.getBody().getData();
		if (!body_data.empty()) {
			write(stdin_pipe[1], body_data.data(), body_data.size());
		}
	}
	close(stdin_pipe[1]);

	// Add CGI stdout to epoll for monitoring
	epoll_manager.add(stdout_pipe[0]);

	// Store CGI process information
	Process cgi_process;
	cgi_process.pid = pid;
	cgi_process.read_fd = stdout_pipe[0];
	cgi_process.completed = false;
	_active_processes[stdout_pipe[0]] = cgi_process;

	// For now, return a "processing" response
	// In a real implementation, the server would need to track this CGI process
	// and send the actual response when the CGI completes
	return utils::makePlainResponse(http::StatusCode::OK, "CGI processing started...",
									http::ContentType::to_string(
										http::ContentType::CONTENT_TEXT_PLAIN));
}

void cgi::Builder::handleCgiEvent(int fd, server::EpollManager& epoll_manager) {
	auto it = _active_processes.find(fd);
	if (it == _active_processes.end()) {
		return;	 // Process not found
	}

	Process& process = it->second;

	// Read available data from CGI process
	char buffer[4096];
	ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

	if (bytes_read > 0) {
		buffer[bytes_read] = '\0';
		process.output += std::string(buffer);
	} else if (bytes_read == 0) {
		// EOF - CGI process finished
		process.completed = true;

		// Wait for child process to avoid zombies
		int status;
		waitpid(process.pid, &status, WNOHANG);

		// Remove from epoll and close fd
		epoll_manager.remove(fd);
		close(fd);

		// Process is now complete, result is in process.output
	} else {
		// Error or would block (EAGAIN/EWOULDBLOCK)
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			// Real error occurred
			process.completed = true;
			epoll_manager.remove(fd);
			close(fd);
		}
	}
}

bool cgi::Builder::hasCgiProcess(int fd) {
	return _active_processes.find(fd) != _active_processes.end();
}

std::string cgi::Builder::getCgiOutput(int fd) {
	auto it = _active_processes.find(fd);
	if (it != _active_processes.end() && it->second.completed) {
		std::string output = it->second.output;
		_active_processes.erase(it);  // Clean up completed process
		return output;
	}
	return "";
}
