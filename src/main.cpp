#include "../include/Config.hpp"
#include "../include/ConfigParser.hpp"

int main(int ac, char **av) {
	try {
		ConfigParser	Parser;
		if (ConfigParser::validateArgument(ac))
			Parser.loadFromFile(av[1]);
		// Config config = Parser.getConfig();
		// Server server(config);
		// server.runServer();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}