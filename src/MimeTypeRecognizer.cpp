
#include "MimeTypeRecognizer.hpp"
#include <iostream>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "../liblogger/liblogger.hpp"

namespace HTTP_Server
{
	MimeTypeRecognizer::MimeTypeRecognizer()
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		initialize_mime_types();
	}

	// Helper function to get the file extension from a file path
	std::string MimeTypeRecognizer::get_file_extension(const std::string &filePath) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		std::filesystem::path path(filePath);
		return path.extension().string(); // Includes the dot (e.g., ".html")
	}

	// Method to get the MIME type information based on file path
	MimeTypeInfo MimeTypeRecognizer::get_mime_type_Info(const std::string &filePath) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		std::string extension = get_file_extension(filePath);
		if (mimeTypeMap.find(extension) != mimeTypeMap.end())
		{
			return mimeTypeMap.at(extension);
		}
		else
		{
			return {"text/html", false}; // Default to binary if unknown
		}
	}

	// Method to check if the file type is binary
	bool MimeTypeRecognizer::is_binary_file(const std::string &filePath) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		return get_mime_type_Info(filePath).is_binary;
	}

	std::string MimeTypeRecognizer::get_mime_type_string(const std::string &filePath) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		std::string extension = get_file_extension(filePath);
		if (mimeTypeMap.find(extension) != mimeTypeMap.end())
		{
			return mimeTypeMap.at(extension).mimeType;
		}
		else
		{
			return "text/html"; // Default to binary if unknown
		}
	}

	// Method to allow adding new MIME types dynamically
	void MimeTypeRecognizer::add_mime_type(const std::string &extension, const std::string &mimeType, bool is_binary)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		mimeTypeMap[extension] = {mimeType, is_binary};
	}

	// Initialize the map with common MIME types
	void MimeTypeRecognizer::initialize_mime_types()
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		mimeTypeMap[".html"] = {"text/html", false};
		mimeTypeMap[".htm"] = {"text/html", false};
		mimeTypeMap[".css"] = {"text/css", false};
		mimeTypeMap[".js"] = {"application/javascript", false};
		mimeTypeMap[".json"] = {"application/json", false};
		mimeTypeMap[".jpg"] = {"image/jpeg", true};
		mimeTypeMap[".jpeg"] = {"image/jpeg", true};
		mimeTypeMap[".png"] = {"image/png", true};
		mimeTypeMap[".gif"] = {"image/gif", true};
		mimeTypeMap[".bmp"] = {"image/bmp", true};
		mimeTypeMap[".ico"] = {"image/x-icon", true};
		mimeTypeMap[".svg"] = {"image/svg+xml", false};
		mimeTypeMap[".txt"] = {"text/plain", false};
		mimeTypeMap[".xml"] = {"application/xml", false};
		mimeTypeMap[".pdf"] = {"application/pdf", true};
		mimeTypeMap[".mp4"] = {"video/mp4", true};
		mimeTypeMap[".webm"] = {"video/webm", true};
		mimeTypeMap[".ogg"] = {"audio/ogg", true};
		mimeTypeMap[".mp3"] = {"audio/mpeg", true};
	}

}
