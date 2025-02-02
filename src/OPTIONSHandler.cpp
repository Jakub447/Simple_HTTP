
#include "OPTIONSHandler.hpp"
#include "MimeTypeRecognizer.hpp"
#include "ResponseCache.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <openssl/sha.h> // Requires OpenSSL for hashing
#include <sys/stat.h>
#include "utils.hpp"


#include "../liblogger/liblogger.hpp"

#include "error_codes.hpp"


namespace HTTP_Server
{



	int OPTIONSHandler::handle_method(const std::string &root_dir, const HTTP_request_info &req_info, const HTTPHeaders &req_headers, HTTPHeaders &resp_headers, HTTP_request_response &resp_info, ResponseCache &response_cache, std::unique_ptr<CacheEntry> &cache_entry, bool &is_served_from_cache)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		lib_logger::LOG(lib_logger::LogLevel::INFO, "serving POST method");
		resp_info.prot_ver = req_info.prot_ver;
		resp_info.resp_code = HTTP_ERR_NO_CONTENT;
		resp_info.status_message = get_srv_error_description((HTTP_error_code)resp_info.resp_code);





		resp_headers.add_header("Allow", "GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS");
		resp_headers.add_header("Content-Length", std::to_string(0));

		return APP_ERR_OK;
	}
}