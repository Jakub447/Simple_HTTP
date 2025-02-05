
#include "PUTHandler.hpp"
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


	static int write_string_to_file(const std::string &content, const std::string &filename, bool is_binary)
	{

		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");

		// Determine the open mode based on is_binary
		std::ios_base::openmode mode = is_binary ? (std::ios::binary | std::ios::out) : std::ios::out;

		// Ensure the directory exists
		try
		{
			std::filesystem::path filePath(filename);
			std::filesystem::create_directories(filePath.parent_path());
		}
		catch (const std::exception &e)
		{

			lib_logger::LOG(lib_logger::LogLevel::ERROR, "Unable to create directories for file: %s", e.what());
			return APP_ERR_NO_DIR;

		}

		// Open the file with the appropriate mode
		std::ofstream file(filename, mode);
		if (!file)
		{
			// Log the error
			lib_logger::LOG(lib_logger::LogLevel::ERROR, "Unable to open file for writing: %s", filename.c_str());
			return APP_ERR_NO_FILE_OPEN;

		}

		// Write the content to the file
		try
		{
			file << content;
			if (!file)
			{
				throw std::ios_base::failure("Failed to write to file.");
			}
		}
		catch (const std::exception &e)
		{

			lib_logger::LOG(lib_logger::LogLevel::ERROR, "Exception while writing to file: %s", e.what());
			file.close();
			return APP_ERR_FILE_WRITE;
		}

		file.close();
		return APP_ERR_OK; // Success

	}

	int PUTHandler::handle_method(const std::string &root_dir, const HTTP_request_info &req_info, const HTTPHeaders &req_headers, HTTPHeaders &resp_headers, HTTP_request_response &resp_info, ResponseCache &response_cache, std::unique_ptr<CacheEntry> &cache_entry, bool &is_served_from_cache)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		lib_logger::LOG(lib_logger::LogLevel::INFO, "serving PUT method");
		resp_info.prot_ver = req_info.prot_ver;
		resp_info.resp_code = HTTP_ERR_OK;
		resp_info.status_message = get_srv_error_description((HTTP_error_code)resp_info.resp_code);

		std::string filename = concatenate_path(root_dir, req_info.URI);

		lib_logger::LOG(lib_logger::LogLevel::DEBUG, "File: %s", filename.c_str());


		MimeTypeRecognizer recognizer;
		MimeTypeInfo file_mime_type_info = recognizer.get_mime_type_Info(filename);

		write_string_to_file(req_info.body, filename, file_mime_type_info.is_binary);

		resp_headers.add_header("Content-Length", std::to_string(0));

		return APP_ERR_OK;
	}
}