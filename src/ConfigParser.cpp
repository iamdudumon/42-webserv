#include "../include/ConfigParser.hpp"

bool ConfigParser::validateArgument(int ac) {
	if (ac > 2)
		throw ConfigException("[Error] Invalid Argument");
	else if (ac == 2)
		return true;
	return false;
}

std::string ConfigParser::readFromFile(std::string& filePath) {
	std::ifstream configFile(filePath.c_str());
	if (!configFile.is_open())
		throw ConfigException("[Error] File open failed: " + filePath);
	std::ostringstream oss;
	oss << configFile.rdbuf();
	return oss.str();
}

void	ConfigParser::tokenize(std::string& readFile) {
	std::string	token;

	for (unsigned long i = 0; i < readFile.size(); i++) {
		char c = readFile[i];
		if (std::isspace(c)) {
			if (!token.empty()) {
				_tokens.push_back(token);
				token.clear();
			}
		}
		else if (c == ';' || c == '{' || c == '}') {
			if (!token.empty()) {
				_tokens.push_back(token);
				token.clear();
			}
			_tokens.push_back(std::string(1, c));
		}
		else if (c == '#') {
			while (readFile[i] != '\n' && i < readFile.size()) i++;
		}
		else
			token += c;
	}
	if (!token.empty())
		_tokens.push_back(token);
}

bool ConfigParser::expectToken(unsigned long i, const std::string& expected) const {
	if (_tokens.at(i) != expected)
		throw ConfigException("[emerg] Invalid configuration: expected '" + expected + "'");
	return true;
}

void ConfigParser::parseListen(Config& tmpConfig, unsigned long& i) {
	char*	end = NULL;
	long	port = std::strtol(_tokens.at(i).c_str(), &end, 10);
	if (*end != 0)
		throw ConfigException("[emerg] Invalid configuration: Listen '" + _tokens.at(i) + "'");
	else if (port < 0 || 65535 < port)
		throw ConfigException("[emerg] Invalid configuration: port range");
	tmpConfig.setListen(port);
	expectToken(++i, ";");
}

void ConfigParser::parseServerName(Config& tmpConfig, unsigned long& i) {
	tmpConfig.setServerName(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseIndex(Config& tmpConfig, unsigned long& i) {
	tmpConfig.setIndex(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseRoot(Config& tmpConfig, unsigned long& i) {
	tmpConfig.setRoot(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseLocationRoot(ConfigLocation& tmp, unsigned long& i) {
	tmp.setRoot(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseLocationIndex(ConfigLocation& tmp,
									  unsigned long& i) {
	tmp.setIndex(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseLocationLimitExcept(ConfigLocation& tmpLocation,
											unsigned long&	i) {
	std::vector<std::string> tmpMethods;
	while (_tokens.at(i) != ";") {
		tmpMethods.push_back(_tokens[i++]);
	}
	tmpLocation.setAllowMethod(tmpMethods);
	expectToken(i, ";");
}

void ConfigParser::parseLocation(Config& tmpConfig, unsigned long& i) {
	std::string		url = _tokens.at(i);
	expectToken(++i, "{");
	ConfigLocation	tmpLocation;	
	while (_tokens.at(++i) != "}") {
		if (_tokens.at(i) == "root")
			parseLocationRoot(tmpLocation, ++i);
		else if (_tokens.at(i) == "index")
			parseLocationIndex(tmpLocation, ++i);
		else if (_tokens.at(i) == "allow_method")
			parseLocationLimitExcept(tmpLocation, ++i);
		else
			throw ConfigException("[emerg] Invalid configuration: Unknown directive " + _tokens.at(i));
	}
	expectToken(i, "}");
	tmpConfig.setLocation(url, tmpLocation);
}

Config ConfigParser::parseServer(unsigned long& i) {
	Config	tmpConfig;
	expectToken(i, "server");
	expectToken(++i, "{");
	while (_tokens.at(++i) != "}") {
		if (_tokens.at(i) == "listen")
			parseListen(tmpConfig, ++i);
		else if (_tokens.at(i) == "server_name")
			parseServerName(tmpConfig, ++i);
		else if (_tokens.at(i) == "index")
			parseIndex(tmpConfig, ++i);
		else if (_tokens.at(i) == "root")
			parseRoot(tmpConfig, ++i);
		else if (_tokens.at(i) == "location")
			parseLocation(tmpConfig, ++i);
		else
			throw ConfigException("[emerg] Invalid configuration: Unknown directive " + _tokens.at(i));
	}
	expectToken(i, "}");
	return tmpConfig;
}

void ConfigParser::parse() {
	try {
		unsigned long	i = 0;
		expectToken(i, "http");
		expectToken(++i, "{");
		while (_tokens.at(++i) != "}") {
			_configs.push_back(parseServer(i));
		}
		expectToken(i, "}");
	}
	catch (const std::out_of_range& e) {
		throw ConfigException("[emerg] Invalid configuration");
	}
}

void ConfigParser::loadFromFile(std::string filePath) {
	std::string	readFile = readFromFile(filePath);
	tokenize(readFile);
	parse();
	//for (unsigned long i = 0; i < _configs.size(); i++) {
	//	std::cout << "port: " << _configs[i].getListen() << '\n'
	//			  << "index: " << _configs[i].getIndex() << '\n'
	//			  << "server_name: " << _configs[i].getServerName() << '\n'
	//			  << "root: " << _configs[i].getRoot() << "\n\n";
	//	for (auto it = _configs[i].getLocation().begin(); it != _configs[i].getLocation().end() ; it++) {
	//		std::cout << "location: " << it->first << "\n";
	//		std::cout << "index: " << it->second.getIndex() << "\n";
	//		std::cout << "root: " << it->second.getRoot() << "\n";
	//		std::cout << "method: ";
	//		for (unsigned long k = 0; k < it->second.getLimitExcept().size(); k++)
	//			std::cout << it->second.getLimitExcept()[k] << " ";
	//		std::cout << "\n\n";
	//	}
	//}
}

// Node("server")
// └── Node("location", ["/admin"])
//     ├── Node("limit_except", ["GET", "POST"])
//     │   └── Node("deny", ["all"])
//     ├── Node("root", ["/var/www/html"])
//     └── Node("index", ["index.html"])
