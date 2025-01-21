#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "src/HTTPServer.hpp"
#include "liblogger/liblogger.hpp"

int main()
{
	//std::string rootDir = "www"; // Default directory
	HTTP_Server::HTTPServer server;

	if (0 != server.server_init())
	{
		printf("server initialization failed! exit");
		return -1;
	}

	server.run();

	return 0;
}
