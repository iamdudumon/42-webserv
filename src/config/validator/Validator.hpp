// Validator.hpp
#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include <set>
#include <string>
#include <vector>

#include "../model/Config.hpp"

namespace config {
	class Validator {
		private:
			static void validatePort(const Config&, std::set<int>&);
			static void validateLocations(const Config&);
			static void validateLocation(const std::string&, const LocationConfig&);

		public:
			static void validate(const std::map<int, Config>&);
	};
}  // namespace config

#endif
