#ifndef CONFIGPARSER_HPP

#define CONFIGPARSER_HPP

#include <cstdlib>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include "Config.hpp"
#include "exception/ConfigException.hpp"

class ConfigParser {
	private:
		std::vector<Config>		 _configs;
		std::vector<std::string> _tokens;

		std::string readFromFile(std::string& filePath);
		void		tokenize(std::string&);
		bool		expectToken(unsigned long, const std::string& expected) const;
		void		parseListen(Config&, unsigned long&);
		void		parseServerName(Config&, unsigned long&);
		void		parseIndex(Config&, unsigned long&);
		void		parseRoot(Config&, unsigned long&);
		void		parseLocationRoot(Config&, const std::string&, unsigned long&);
		void		parseLocationIndex(Config&, const std::string&, unsigned long&);
		void		parseLocationAllowMethod(Config&, const std::string&, unsigned long&);
		void		parseLocation(Config&, unsigned long&);
		Config		parseServer(unsigned long&);
		void		parse();

	public:
		ConfigParser() {}
		~ConfigParser() {}

		static bool validateArgument(int);
		void		loadFromFile(std::string filePath);
};

#endif
