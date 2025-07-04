#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>

#include "ConfigLocation.hpp"
#include "ConfigParser.hpp"

class Config {
	private:
		int									  _listen;
		std::string							  _server_name;
		std::string							  _index;
		std::string							  _root;
		std::map<std::string, ConfigLocation> _location;

	public:
		Config();
		Config(const Config&) {}
		Config& operator=(const Config&);
		~Config() {}
		int	 getListen();
		void setListen(int listen);
		void setServerName(const std::string& serverName);
		void setIndex(const std::string& index);
		void setRoot(const std::string& root);
		void setLocation(const std::string&	   path,
						 const ConfigLocation& location);
};

#endif
