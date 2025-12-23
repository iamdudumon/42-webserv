#ifndef CLIENT_CLIENT_MANAGER_HPP
#define CLIENT_CLIENT_MANAGER_HPP

#include <map>
#include <vector>

#include "Client.hpp"

namespace client {
	class ClientManager {
		private:
			std::map<int, Client*> _clients;

		public:
			ClientManager();
			~ClientManager();

			Client* getClient(int);
			Client* ensureClient(int, const config::Config*);
			void removeClient(int);

			std::vector<int> checkTimeouts();
	};
}

#endif
