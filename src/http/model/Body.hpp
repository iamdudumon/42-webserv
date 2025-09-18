// Body.hpp
#ifndef HTTP_MODEL_BODY_HPP
#define HTTP_MODEL_BODY_HPP

#include <vector>

#include "../Enums.hpp"

namespace http {
	class Body {
		private:
			std::vector<unsigned char> _data;
			http::ContentType::Value _type;
			size_t _length;

		public:
			Body();
			~Body();
			Body(const Body&);
			Body& operator=(const Body&);

			const std::vector<unsigned char>& getData() const;
			http::ContentType::Value getType() const;
			size_t getLength() const;

			void setType(http::ContentType::Value);
			void setLength(size_t);

			void append(const char*, size_t);
	};
}  // namespace http

#endif
