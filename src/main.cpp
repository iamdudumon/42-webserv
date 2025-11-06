#include <iostream>

#include "config/model/Config.hpp"
#include "config/parser/Parser.hpp"
#include "server/Server.hpp"

int main(int argc, char* argv[]) {
	try {
		config::Parser parser;
		if (parser.validateArgument(argc)) parser.loadFromFile(argv[1]);
		std::map<int, config::Config> configs = parser.getConfigs();
		server::Server server(configs);

		server.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}
