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

void ConfigParser::loadFromFile(std::string filePath) {
	std::string	readFile = readFromFile(filePath);
	tokenize(readFile);
	//parse();
	//for (unsigned long i = 0; i < _tokens.size(); i++)
	//{
	//	std::cout << _tokens[i] << "\n";
	//}
}

// Node("server")
// └── Node("location", ["/admin"])
//     ├── Node("limit_except", ["GET", "POST"])
//     │   └── Node("deny", ["all"])
//     ├── Node("root", ["/var/www/html"])
//     └── Node("index", ["index.html"])
