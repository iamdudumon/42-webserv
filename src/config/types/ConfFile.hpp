// ConfFile.hpp
#ifndef CONFFILE_HPP
#define CONFFILE_HPP

namespace ConfFile {
	inline const char* DEFAULT_PATH() { return "default.conf"; }
	static const long long MAX_ALLOWABLE_SIZE = 2LL * 1024 * 1024 * 1024; // 2GB
}

#endif
