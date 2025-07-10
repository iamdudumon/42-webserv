#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <vector>

#include "exception/ConfigException.hpp"

struct ConfigLocation {
	std::string	_root;
	std::string	_index;
	std::vector<std::string> _allow_method;
};

class Config {
	private:
		int									  _listen;
		std::string							  _server_name;
		std::string							  _index;
		std::string							  _root;
		std::map<std::string, ConfigLocation> _location;

	public:
		Config();
		Config(const Config&);
		Config& operator=(const Config&);
		~Config() {}

		// setter
		void setListen(int listen);
		void setServerName(const std::string& serverName);
		void setIndex(const std::string& index);
		void setRoot(const std::string& root);
		void setLocation(const std::string& path);
		void setLocationRoot(const std::string& path, const std::string& root);
		void setLocationIndex(const std::string& path, const std::string& index);
		void setLocationAllowMethod(const std::string& path, const std::vector<std::string>& methods);

		// getter
		int											 getListen() const;
		const std::string&							 getServerName() const;
		const std::string&							 getIndex() const;
		const std::string&							 getRoot() const;
		const std::map<std::string, ConfigLocation>& getLocation() const;
		const std::string&							 getLocationRoot(const std::string& path) const;
		const std::string&							 getLocationIndex(const std::string& path) const;
		const std::vector<std::string>&				 getLocationAllowMethod(const std::string& path) const;
	};

#endif
