#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Config.hpp"

class ConfigParser {
	private:
		Config					 _config;
		std::vector<std::string> _tokens;

		std::string readFromFile(std::string& filePath);
		void		tokenize(std::string&);
		//bool		validateIdx(unsigned long) const;
		bool		expectToken(unsigned long, const std::string& expected) const;
		void		parseListen(unsigned long&);
		void		parseServerName(unsigned long&);
		void		parseIndex(unsigned long&);
		void		parseRoot(unsigned long&);
		void		parseLocationRoot(ConfigLocation&, unsigned long&);
		void		parseLocationIndex(ConfigLocation&, unsigned long&);
		void		parseLocationLimitExcept(ConfigLocation&, unsigned long&);
		void		parseLocation(unsigned long&);
		void		parse();

	public:
		ConfigParser() {}
		~ConfigParser() {}

		// Config	getConfig();
		static bool validateArgument(int);
		void		loadFromFile(std::string filePath);
		class ConfigParserException : public std::exception {
			private:
				std::string _exception;

			public:
				ConfigParserException(std::string e) { _exception = e; }
				~ConfigParserException() throw() {}
				virtual const char* what() const throw() {
					return _exception.c_str();
				}
		};
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