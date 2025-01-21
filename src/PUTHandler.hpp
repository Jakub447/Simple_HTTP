#ifndef PUT_HANDLER_HPP
#define PUT_HANDLER_HPP

#include "IMethodHandler.hpp"
#include <fstream>
#include <sstream>
#include <memory>

namespace HTTP_Server
{

	class PUTHandler : public IMethodHandler
	{
	public:
		int handle_method(const std::string &root_dir, const HTTP_request_info &req_info, const HTTPHeaders &req_headers, HTTPHeaders &resp_headers, HTTP_request_response &resp_info, ResponseCache& response_cache, std::unique_ptr<CacheEntry> & cache_entry, bool& is_served_from_cache) override;
	};

}

#endif // GET_HANDLER_HPP
