// ConfigValidator.hpp
#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include <set>

#include "../utils/str_utils.hpp"
#include "exception/ConfigException.hpp"
#include "model/Config.hpp"

class ConfigValidator {
	private:
		static void validatePort(const Config&, std::set<int>&);
		static void validateLocations(const Config&);
		static void validateLocation(const std::string&, const ConfigLocation&);

	public:
		static void validate(const std::vector<Config>& configs);
};

#endif
