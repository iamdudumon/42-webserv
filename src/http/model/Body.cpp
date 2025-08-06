#include "Body.hpp"

Body::Body() {}

Body::~Body() {}

Body::Body(const Body& copy) : _data(copy._data), _type(copy._type) {}

Body& Body::operator=(const Body& copy) {
	if (this != &copy) {
		this->_data = copy._data;
		this->_type = copy._type;
	}
	return (*this);
}

const std::vector<unsigned char>& Body::getData() const {
	return const_cast<std::vector<unsigned char>&>(_data);
}

HTTP::ContentType::Value Body::getType() const { return _type; }

void Body::setType(HTTP::ContentType::Value type) { this->_type = type; }

void Body::append(const char* data, size_t len) {
	_data.insert(_data.end(), reinterpret_cast<const unsigned char*>(data),
				 reinterpret_cast<const unsigned char*>(data) + len);
}
