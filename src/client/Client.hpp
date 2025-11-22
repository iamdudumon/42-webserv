#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../config/model/Config.hpp"
#include "../http/parser/Parser.hpp"

namespace client {
	class Client {
		public:
			enum State {
				None,
				Receiving,
				Processing,
				Sending,
				Closing
			};

		private:
			int _fd;
			http::Parser* _parser;
			const config::Config* _cgiConfig;
			bool _keepAlive;
			State _state;

		public:
			Client(int, const config::Config*);
			~Client();

			http::Parser* getParser();

			const config::Config* getCgiConfig() const;
			void setCgiConfig(const config::Config*);
			bool isKeepAlive() const;
			void setKeepAlive(bool);

			State getState() const;
			void setState(State);
	};
}  // namespace client

#endif
