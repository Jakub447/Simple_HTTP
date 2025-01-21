#ifndef HTTP_METHOD_FACTORY_H
#define HTTP_METHOD_FACTORY_H

#include <string>
#include <memory>
#include "IMethodHandler.hpp"
#include "HTTPMethodFactory.hpp"
#include "HTTP_connection_info.hpp"

namespace HTTP_Server
{

	class HTTPMethodFactory
	{
	public:
		static std::unique_ptr<IMethodHandler> create_handler(const HTTPMethod &method);
	};

}
#endif // HTTP_METHOD_FACTORY_H