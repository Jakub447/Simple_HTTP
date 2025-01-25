
#include "GETHandler.hpp"
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

namespace HTTP_Server
{

	// Function to convert FileOpenMode to std::ios flags
	static std::ios_base::openmode to_open_mode(bool is_binary)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		if (is_binary)
		{
			return std::ios::in | std::ios::binary;
		}
		else
		{
			return std::ios::in;
		}
	}

	static std::string read_file_to_string(const std::string &filename, bool is_binary)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		std::ifstream file(filename, to_open_mode(is_binary));
		if (!file)
		{
			// throw std::runtime_error("Unable to open file");

			// Log the error and return an empty string
			lib_logger::LOG(lib_logger::LogLevel::ERROR,"Unable to open file: %s", filename);
			return ""; // Return empty content if the file can't be read
		}
		return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}

	// Function to generate an ETag based on the content of a file
	static std::string generate_ETag(const std::string &filename)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		std::ifstream file(filename, std::ios::binary);
		if (!file)
			return "";

		// Read file content into a string
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// Compute SHA256 hash
		unsigned char hash[SHA256_DIGEST_LENGTH];
		SHA256(reinterpret_cast<const unsigned char *>(content.data()), content.size(), hash);

		// Convert hash to hex string
		std::ostringstream oss;
		for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		{
			oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
		}

		return oss.str();
	}

	static bool should_cache_response(const HTTPHeaders &headers)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		// Check for a `Cache-Control` header with `no-store` directive
		if (headers.has_header("Cache-Control"))
		{
			std::string cache_control = headers.get_header("Cache-Control").front();
			if (cache_control.find("no-store") != std::string::npos)
			{
				return false;
			}
		}

		// Other conditions to determine caching can go here
		return true;
	}

	int GETHandler::handle_method(const std::string &root_dir, const HTTP_request_info &req_info, const HTTPHeaders &req_headers, HTTPHeaders &resp_headers, HTTP_request_response &resp_info, ResponseCache &response_cache, std::unique_ptr<CacheEntry> &cache_entry, bool &is_served_from_cache)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		lib_logger::LOG(lib_logger::LogLevel::INFO,"serving GET method");
		resp_info.prot_ver = req_info.prot_ver;
		resp_info.resp_code = 200;
		resp_info.status_message = "OK";

		std::string filename = concatenate_path(root_dir, req_info.URI);
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"filename after fun: %s", filename.c_str());

		std::string currentETag = generate_ETag(filename);
		std::string cache_key = req_info.URI; // Use the URI as the cache key

		// Check cache first
		auto cached_entry = response_cache.get(cache_key);
		if (cached_entry && response_cache.validate_cache_entry(cached_entry, req_headers))
		{

			lib_logger::LOG(lib_logger::LogLevel::DEBUG,"Serving from cache!");
			is_served_from_cache = true;
			cache_entry = std::make_unique<CacheEntry>(cached_entry.value());

			// resp_info.resp_code = 304;
			// resp_info.status_message = "NOT MODIFIED";
			resp_info.resp_final_body = cached_entry->body;
			return 0; // Successfully served from cache
		}

		resp_headers.add_header("ETag", currentETag);
		resp_headers.add_header("Cache-Control", "max-age=60");

		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"File: %s", filename.c_str());

		MimeTypeRecognizer recognizer;
		MimeTypeInfo file_mime_type_info = recognizer.get_mime_type_Info(filename);
		resp_headers.add_header("Content-Type", file_mime_type_info.mimeType);
		resp_info.resp_final_body = read_file_to_string(filename, file_mime_type_info.is_binary);

		if(resp_info.resp_final_body == "")
		{
			resp_info.resp_code = 404;
			resp_info.status_message = "NOT FOUND";

			lib_logger::LOG(lib_logger::LogLevel::WARNING,"404 NOT FOUND");
		}

		resp_headers.add_header("Content-Length", std::to_string(resp_info.resp_final_body.length()));

		// Decide if the response should be cached based on headers or other conditions
		if (resp_info.resp_final_body != "" && should_cache_response(resp_headers))
		{
			// Call put to store in the cache
			response_cache.put(cache_key, resp_info.resp_final_body, resp_headers, currentETag);
		}

		return 0;
	}
}