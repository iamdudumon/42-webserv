#include "../include/Server.hpp"


int main(){
	Config config(8082);
	Server server(config);
	server.runServer();
}