// ConfigValidator.hpp
#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include <set>

#include "../utils/str_utils.hpp"
#include "exception/ConfigException.hpp"
#include "model/Config.hpp"

class ConfigValidator {
	private:

	public:
		static void validate(const std::vector<Config>& configs);
};

#endif
