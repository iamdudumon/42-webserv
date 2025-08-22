// ConfFile.hpp
#ifndef CONFFILE_HPP
#define CONFFILE_HPP

namespace ConfFile {
	inline const char* DEFAULT_PATH() { return "default.conf"; }
	static const long long DEFAULT_CLIENT_MAX_BODY_SIZE = 1024 * 1024;
	static const long long CLIENT_BODY_SIZE_LIMIT = 2LL * 1024 * 1024 * 1024; // 2GB
}

#endif
