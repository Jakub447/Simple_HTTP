#include "HTTP_connection_info.hpp"

#include "../liblogger/liblogger.hpp"

namespace HTTP_Server
{

	std::string http_method_to_string(HTTPMethod method)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		switch (method)
		{
		case GET:
			return "GET";
		// case POST:
		//	return "POST";
		case PUT:
			return "PUT";
		// case DELETE:
		//	return "DELETE";
		// case PATCH:
		//	return "PATCH";
		// case HEAD:
		//	return "HEAD";
		// case OPTIONS:
		//	return "OPTIONS";
		default:
			return "UNKNOWN";
		}
	}
}