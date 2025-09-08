// IoException.hpp
#ifndef IO_EXCEPTION_HPP
#define IO_EXCEPTION_HPP

#include <exception>
#include <iostream>

class IoException : public std::exception {
	private:
		std::string _exception;

	public:
		IoException(std::string e) { _exception = e; }
		~IoException() throw() {}
		virtual const char* what() const throw() { return _exception.c_str(); }
};

#endif
