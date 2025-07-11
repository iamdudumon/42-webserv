#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include "Config.hpp"
#include "exception/ConfigException.hpp"

class ConfigValidator {
	public:
		static void validate(const Config& config);
};

#endif