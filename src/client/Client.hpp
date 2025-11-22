#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../config/model/Config.hpp"
#include "../http/parser/Parser.hpp"

namespace client {
	class Client {
		private:
			int _fd;
			http::Parser* _parser;
			const config::Config* _cgiConfig;
			bool _keepAlive;

		public:
			Client(int, const config::Config*);
			~Client();

			http::Parser* getParser();
			const config::Config* getCgiConfig() const;
			bool isKeepAlive() const;
			void setCgiConfig(const config::Config*);
			void setKeepAlive(bool);
	};
}  // namespace client

#endif
