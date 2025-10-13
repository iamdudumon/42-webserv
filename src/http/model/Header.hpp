// Header.hpp
#ifndef HTTP_MODEL_HEADER_HPP
#define HTTP_MODEL_HEADER_HPP

#include <map>
#include <string>

namespace http {
	class Header {
		private:
			std::map<std::string, std::string> _headers;

		public:
			Header();
			~Header();
			Header(const Header&);
			Header& operator=(const Header&);

			const std::map<std::string, std::string>& getHeaders() const;

			const std::string& get(const std::string&) const;
			void set(const std::string&, const std::string&);
	};
}  // namespace http

#endif
