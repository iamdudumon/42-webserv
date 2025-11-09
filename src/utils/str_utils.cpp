#include "str_utils.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

std::string int_tostr(int num) {
	std::stringstream s;

	s << num;
	return (s.str());
}

int str_toint(const std::string& str) {
	std::stringstream s;
	int num;

	s << str;
	s >> num;
	return (num);
}

std::string to_lower(const std::string& str) {
	std::string lower_str = str;
	std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
	return lower_str;
}

std::string trim(const std::string& value) {
	size_t start = value.find_first_not_of(" \t");
	if (start == std::string::npos) return std::string();
	size_t end = value.find_last_not_of(" \t");
	return value.substr(start, end - start + 1);
}
