
#include "PATCHHandler.hpp"
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

	static bool file_exists(const std::string &filename)
	{
		struct stat buffer;
		return (stat(filename.c_str(), &buffer) == 0); // Returns true if the file exists
	}

	static int write_string_to_file(const std::string &content, const std::string &filename, bool is_binary)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");

		if (!file_exists(filename))
		{
			lib_logger::LOG(lib_logger::LogLevel::ERROR, "file does not exist");
			return APP_ERR_FAILURE; // Exit if file doesn't exist
		}

		// Determine the open mode based on is_binary
		std::ios_base::openmode mode = is_binary ? (std::ios::binary | std::ios::app) : std::ios::app;
		//std::ios_base::openmode mode = std::ios::out | std::ios::app;

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
		std::ofstream file(filename, std::ios::out | std::ios::app);
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

	int PATCHHandler::handle_method(const std::string &root_dir, const HTTP_request_info &req_info, const HTTPHeaders &req_headers, HTTPHeaders &resp_headers, HTTP_request_response &resp_info, ResponseCache &response_cache, std::unique_ptr<CacheEntry> &cache_entry, bool &is_served_from_cache)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		lib_logger::LOG(lib_logger::LogLevel::INFO, "serving POST method");
		resp_info.prot_ver = req_info.prot_ver;
		resp_info.resp_code = HTTP_ERR_CREAT;
		resp_info.status_message = get_srv_error_description((HTTP_error_code)resp_info.resp_code);

		std::string filename = concatenate_path(root_dir, req_info.URI);

		lib_logger::LOG(lib_logger::LogLevel::DEBUG, "File: %s", filename.c_str());


		MimeTypeRecognizer recognizer;
		MimeTypeInfo file_mime_type_info = recognizer.get_mime_type_Info(filename);


		if(APP_ERR_OK != write_string_to_file(req_info.body, filename, file_mime_type_info.is_binary)){
			resp_info.resp_code = HTTP_ERR_INTERNAL_SERVER;
			resp_info.status_message = get_srv_error_description((HTTP_error_code)resp_info.resp_code);	
		}


		resp_headers.add_header("Content-Length", std::to_string(0));

		return APP_ERR_OK;
	}
}