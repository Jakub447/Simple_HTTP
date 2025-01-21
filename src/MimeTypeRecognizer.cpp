
#include "MimeTypeRecognizer.hpp"
#include <iostream>
#include <unordered_map>
#include <string>
#include <filesystem>

namespace HTTP_Server
{
	MimeTypeRecognizer::MimeTypeRecognizer()
	{
		initializeMimeTypes();
	}

	// Helper function to get the file extension from a file path
	std::string MimeTypeRecognizer::getFileExtension(const std::string &filePath) const
	{
		std::filesystem::path path(filePath);
		return path.extension().string(); // Includes the dot (e.g., ".html")
	}

	// Method to get the MIME type information based on file path
	MimeTypeInfo MimeTypeRecognizer::getMimeTypeInfo(const std::string &filePath) const
	{
		std::string extension = getFileExtension(filePath);
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
	bool MimeTypeRecognizer::isBinaryFile(const std::string &filePath) const
	{
		return getMimeTypeInfo(filePath).is_binary;
	}

	std::string MimeTypeRecognizer::getMimeTypeString(const std::string &filePath) const
	{
		std::string extension = getFileExtension(filePath);
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
	void MimeTypeRecognizer::addMimeType(const std::string &extension, const std::string &mimeType, bool is_binary)
	{
		mimeTypeMap[extension] = {mimeType, is_binary};
	}

	// Initialize the map with common MIME types
	void MimeTypeRecognizer::initializeMimeTypes()
	{
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
