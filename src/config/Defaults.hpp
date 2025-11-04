// Defaults.hpp
#ifndef CONFIG_DEFAULTS_HPP
#define CONFIG_DEFAULTS_HPP

namespace config {
	namespace defaults {
		inline const char* PATH() {
			return "default.conf";
		}
		inline const char* SERVER_NAME() {
			return "_";
		}
		static const long long CLIENT_MAX_BODY_SIZE = 1024 * 1024;
		static const long long LIMIT_CLIENT_MAX_BODY_SIZE = 2LL * 1024 * 1024 * 1024;  // 2GB
	}
}  // namespace config

#endif
