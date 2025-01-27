#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "src/HTTPServer.hpp"
#include "liblogger/liblogger.hpp"

#include "src/error_codes.hpp"

int main()
{
	//std::string rootDir = "www"; // Default directory
	HTTP_Server::HTTPServer server;
	
	HTTP_Server::app_error_code ret = HTTP_Server::APP_ERR_OK;

	ret = (HTTP_Server::app_error_code)server.server_init();
	if (0 !=  ret)
	{
		printf("server initialization failed! exit: %s(%d)",HTTP_Server::get_app_error_description(ret).c_str(),ret);
		return ret;
	}

	server.run();

	return ret;
}
