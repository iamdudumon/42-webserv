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
		Header(const Header&);
		Header& operator=(const Header&);

		const std::string& get(std::string) const;
		void			   set(std::string, std::string);
};

#endif
