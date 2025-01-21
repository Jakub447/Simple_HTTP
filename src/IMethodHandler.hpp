#ifndef IMETHOD_HANDLER_H
#define IMETHOD_HANDLER_H

#include "HTTP_connection_info.hpp"
#include "HttpHeaders.hpp"
#include "ResponseCache.hpp"
#include <memory>
#include <string>

namespace HTTP_Server
{

	class IMethodHandler
	{
	public:
		virtual ~IMethodHandler() = default;

		// Pure virtual method to build the response based on request data
		virtual int handle_method(const std::string &root_dir, const HTTP_request_info &req_info, const HTTPHeaders &req_headers, HTTPHeaders &resp_headers, HTTP_request_response &resp_info, ResponseCache& response_cache, std::unique_ptr<CacheEntry> & cache_entry, bool& is_served_from_cache) = 0;
	};
}
#endif // IMETHOD_HANDLER_H