// ConfigValidator.hpp
#pragma once

#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include "model/Config.hpp"
#include "exception/ConfigException.hpp"

class ConfigValidator {
	public:
		static void validate(const Config&);
};

#endif