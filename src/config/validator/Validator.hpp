// Validator.hpp
#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include <set>

#include "../exception/Exception.hpp"
#include "../model/Config.hpp"

namespace config {
	class Validator {
		private:
			static void validatePort(const Config&, std::set<int>&);
			static void validateLocations(const Config&);
			static void validateLocation(const std::string&, const LocationConfig&);

		public:
			static void validate(const std::vector<Config>&);
	};
}  // namespace config

#endif
