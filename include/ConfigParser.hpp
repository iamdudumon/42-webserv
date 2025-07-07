#ifndef CONFIGPARSER_HPP

#define CONFIGPARSER_HPP

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Config.hpp"
#include "ConfigException.hpp"

class ConfigParser {
	private:
		std::vector<Config>		 _configs;
		std::vector<std::string> _tokens;

		std::string readFromFile(std::string& filePath);
		void		tokenize(std::string&);
		//bool		validateIdx(unsigned long) const;
		bool		expectToken(unsigned long, const std::string& expected) const;
		void		parseListen(Config&, unsigned long&);
		void		parseServerName(Config&, unsigned long&);
		void		parseIndex(Config&, unsigned long&);
		void		parseRoot(Config&, unsigned long&);
		void		parseLocationRoot(ConfigLocation&, unsigned long&);
		void		parseLocationIndex(ConfigLocation&, unsigned long&);
		void		parseLocationLimitExcept(ConfigLocation&, unsigned long&);
		void		parseLocation(Config&, unsigned long&);
		Config		parseServer(unsigned long&);
		void		parse();

	public:
		ConfigParser() {}
		~ConfigParser() {}

		// Config	getConfig();
		static bool validateArgument(int);
		void		loadFromFile(std::string filePath);
};

#endif

// server {
//     listen 80;
//     server_name example.com;

//     location / {
//         root /var/www/html;
//         index index.html;
//     }
// }