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
			Client(const Client&);
			Client& operator=(const Client&);

			int _fd;
			http::Parser* _parser;
			const config::Config* _cgiConfig;
			bool _keepAlive;
			time_t _lastActivity;
			State _state;
			http::Packet* _request;
			http::Packet* _response;

		public:
			Client(int, const config::Config*);
			~Client();

			int getFd() const;
			std::string readSocket(bool&) const;

			const config::Config* getCgiConfig() const;
			void setCgiConfig(const config::Config*);
			bool isKeepAlive() const;
			void setKeepAlive(bool);

			State getState() const;
			void setState(State);

			bool hasRequest() const;
			http::Packet* takeRequest();

			http::Packet* getResponse() const;
			void setResponse(http::Packet*);
			void clearResponse();

			void updateLastActivity();
			bool isTimedOut(time_t) const;

			void processData(const std::string&);
			bool hasUnconsumedInput() const;
			void markEndOfInput();
	};
}  // namespace client

#endif
