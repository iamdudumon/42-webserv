// Header.hpp
#pragma once
#ifndef HEADER_HPP
#define HEADER_HPP

#include <map>
#include <string>

class Header {
	private:
		std::map<std::string, std::string> _headers;

	public:
		Header();
		~Header();
		Header(const Header& copy);
		Header& operator=(const Header& copy);

		const std::string& get(std::string key) const;
		void			   set(std::string key, std::string value);
};

#endif
