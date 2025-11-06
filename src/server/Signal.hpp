#ifndef SERVER_SIGNAL_SIGNALHANDLER_HPP
#define SERVER_SIGNAL_SIGNALHANDLER_HPP

#include <signal.h>

#include "../handler/cgi/ProcessManager.hpp"
#include "exception/Exception.hpp"

namespace {
	void sigchldHandler(int) {
		int status;
		while (waitpid(-1, &status, WNOHANG) > 0) {
		}
	}
}

namespace server {
	void signalInstall() {
		struct sigaction sa;
		sa.sa_handler = sigchldHandler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
		if (sigaction(SIGCHLD, &sa, NULL) == -1)
			throw server::Exception("sigaction SIGCHLD failed");

		sa.sa_handler = SIG_IGN;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		if (sigaction(SIGPIPE, &sa, NULL) == -1)
			throw server::Exception("sigaction SIGPIPE failed");
	}

}

#endif
