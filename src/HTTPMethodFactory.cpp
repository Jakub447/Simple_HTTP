#include <string>
#include <memory>
#include "HTTPMethodFactory.hpp"
#include "HTTP_connection_info.hpp"
#include "GETHandler.hpp"
#include "PUTHandler.hpp"

namespace HTTP_Server
{

	std::unique_ptr<IMethodHandler> HTTPMethodFactory::create_handler(const HTTPMethod &method)
	{

		switch (method)
		{
		case GET:
			return std::make_unique<GETHandler>();
		case PUT:
			return std::make_unique<PUTHandler>();

		default:
			std::cout << "Invalid option!" << std::endl;
			break;
		}
		return nullptr;
	}

}