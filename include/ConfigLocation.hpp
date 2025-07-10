#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>

class ConfigLocation {
	private:
		std::string	_root;
		std::string	_index;
		std::vector<std::string> _allow_method;
	public:
		ConfigLocation() {}
		~ConfigLocation() {}
		const std::string& getRoot() const;
    	const std::string& getIndex() const;
    	const std::vector<std::string>& getLimitExcept() const;
		
		void	setRoot(const std::string& root);
		void	setIndex(const std::string& index);
		void	setAllowMethod(const std::vector<std::string>& methods);
};

#endif
