// EventHandler.hpp
#ifndef HANDLER_EVENTHANDLER_HPP
#define HANDLER_EVENTHANDLER_HPP

#include <stdint.h>

#include <map>
#include <string>

#include "../config/model/Config.hpp"
#include "../http/model/Packet.hpp"
#include "../http/parser/Parser.hpp"
#include "../router/Router.hpp"
#include "RequestHandler.hpp"
#include "cgi/ProcessManager.hpp"

namespace server {
	class EpollManager;
}

namespace handler {
	class EventHandler {
		public:
			struct Result {
					int fd;
					http::Packet* packet;
					std::string raw;
					bool useRaw;
					bool keepAlive;
					bool closeAfterSend;
					int closeFd;
					Result() :
						fd(-1),
						packet(NULL),
						raw(),
						useRaw(false),
						keepAlive(false),
						closeAfterSend(false),
						closeFd(-1) {}
					Result(const Result& other) :
						fd(other.fd),
						packet(other.packet ? new http::Packet(*other.packet) : NULL),
						raw(other.raw),
						useRaw(other.useRaw),
						keepAlive(other.keepAlive),
						closeAfterSend(other.closeAfterSend),
						closeFd(other.closeFd) {}
					Result& operator=(const Result& other) {
						if (this != &other) {
							fd = other.fd;
							raw = other.raw;
							useRaw = other.useRaw;
							keepAlive = other.keepAlive;
							closeAfterSend = other.closeAfterSend;
							closeFd = other.closeFd;
							delete packet;
							packet = other.packet ? new http::Packet(*other.packet) : NULL;
						}
						return *this;
					}
					~Result() {
						delete packet;
					}
					void setPacket(const http::Packet& value) {
						delete packet;
						packet = new http::Packet(value);
					}
					void clearPacket() {
						delete packet;
						packet = NULL;
					}
			};

		private:
			router::Router _router;
			RequestHandler _requestHandler;
			cgi::ProcessManager _cgiProcessManager;
			std::map<int, http::Parser*> _parsers;
			struct CgiContext {
					const config::Config* config;
					bool keepAlive;
					CgiContext() : config(NULL), keepAlive(false) {}
					CgiContext(const config::Config* cfg, bool ka) : config(cfg), keepAlive(ka) {}
			};
			std::map<int, CgiContext> _cgiContexts;

			http::Parser* ensureParser(int, const config::Config*);
			std::string readSocket(int) const;
			Result handleClientEvent(int, uint32_t, const config::Config*, server::EpollManager&);
			Result handleCgiEvent(int, uint32_t, const config::Config*, server::EpollManager&);

		public:
			EventHandler();
			~EventHandler();

			Result handleEvent(int, uint32_t, const config::Config*, server::EpollManager&);
			void cleanup(int, server::EpollManager&);
	};
}  // namespace handler

#endif
