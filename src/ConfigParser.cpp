#include "../include/ConfigParser.hpp"

bool ConfigParser::validateArgument(int ac) {
	if (ac > 2)
		throw ConfigParser::ConfigParserException("[Error] Invalid Argument");
	else if (ac == 2)
		return true;
	return false;
}

// Config	ConfigParser::getConfig()
//{
//	return _config;
// }

std::string ConfigParser::readFromFile(std::string& filePath) {
	std::ifstream configFile(filePath.c_str());
	if (!configFile.is_open())
		throw ConfigParserException("[Error] File open failed: " + filePath);
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
		throw ConfigParserException("[emerg] Invalid configuration: expected '" + expected + "'");
	return true;
}

void ConfigParser::parseListen(unsigned long& i) {
	char*	end = NULL;
	long	port = std::strtol(_tokens.at(i).c_str(), &end, 10);
	if (*end != 0)
		throw ConfigParserException("[emerg] Invalid configuration: Listen '" + _tokens.at(i) + "'");
	else if (port < 0 || 65535 < port)
		throw ConfigParserException("[emerg] Invalid configuration: port range");
	_config.setListen(port);
	expectToken(++i, ";");
}

void ConfigParser::parseServerName(unsigned long& i) {
	_config.setServerName(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseIndex(unsigned long& i) {
	_config.setIndex(_tokens.at(i));
	expectToken(++i, ";");
}

void ConfigParser::parseRoot(unsigned long& i) {
	_config.setRoot(_tokens.at(i));
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
	tmpLocation.setLimitExcept(tmpMethods);
	expectToken(i, ";");
}

void ConfigParser::parseLocation(unsigned long& i) {
	std::string		url = _tokens.at(i);
	expectToken(++i, "{");
	
	ConfigLocation	tmpLocation;
	while (_tokens.at(++i) != "}") {
		if (_tokens.at(i) == "root")
			parseLocationRoot(tmpLocation, ++i);
		else if (_tokens.at(i) == "index")
			parseLocationIndex(tmpLocation, ++i);
		else if (_tokens.at(i) == "limit_except")
			parseLocationLimitExcept(tmpLocation, ++i);
		else
			throw ConfigParserException("[emerg] Invalid configuration");
	}
	expectToken(i, "}");
	_config.setLocation(url, tmpLocation);
}

void ConfigParser::parse() {
	try {
		unsigned long i = 0;
		expectToken(i, "server");
		expectToken(++i, "{");
		while (_tokens.at(++i) != "}") {
			if (_tokens.at(i) == "listen")
				parseListen(++i);
			else if (_tokens.at(i) == "server_name")
				parseServerName(++i);
			else if (_tokens.at(i) == "index")
				parseIndex(++i);
			else if (_tokens.at(i) == "root")
				parseRoot(++i);
			else if (_tokens.at(i) == "location")
				parseLocation(++i);
			else
				throw ConfigParserException("[emerg] Invalid configuration: Unknown directive " + _tokens.at(i));
		}
		expectToken(i, "}");
	}
	catch (const std::out_of_range& e) {
		throw ConfigParserException("[emerg] Invalid configuration");
	}
}

void ConfigParser::loadFromFile(std::string filePath) {
	std::string	readFile = readFromFile(filePath);
	tokenize(readFile);
	parse();
	std::cout << "port: " << _config.getListen() << '\n'
			  << "index: " << _config.getIndex() << '\n'
			  << "server_name: " << _config.getServerName() << '\n'
			  << "root: " << _config.getRoot() << "\n\n";
	for (auto i = _config.getLocation().begin(); i != _config.getLocation().end() ; i++) {
		std::cout << "location: " << i->first << "\n";
		std::cout << "index: " << i->second.getIndex() << "\n";
		std::cout << "root: " << i->second.getRoot() << "\n";
		std::cout << "method: ";
		for (unsigned long k = 0; k < i->second.getLimitExcept().size(); k++)
			std::cout << i->second.getLimitExcept()[k] << " ";
		std::cout << "\n\n";
	}
	//for (unsigned long i = 0; i < _tokens.size(); i++)
	//{
	//	std::cout << _tokens.at(i) << "\n";
	//}
}

// Node("server")
// └── Node("location", ["/admin"])
//     ├── Node("limit_except", ["GET", "POST"])
//     │   └── Node("deny", ["all"])
//     ├── Node("root", ["/var/www/html"])
//     └── Node("index", ["index.html"])
