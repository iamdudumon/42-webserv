// ConfigValidator.hpp
#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include "exception/ConfigException.hpp"
#include "model/Config.hpp"

class ConfigValidator {
	public:
		static void validate(const Config&);
};

#endif
