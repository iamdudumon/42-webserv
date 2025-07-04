#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>

class ConfigLocation {
	private:
		std::string	_root;
		std::string	_index;
		std::vector<std::string> _limit_except;
	public:
		ConfigLocation() {}
		~ConfigLocation() {}
		void	setRoot(const std::string& root);
		void	setIndex(const std::string& index);
		void	setLimitExcept(const std::vector<std::string>& methods);
};

#endif
