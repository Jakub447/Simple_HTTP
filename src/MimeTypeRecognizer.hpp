#ifndef MIMETYPE_RECOGNIZER_H
#define MIMETYPE_RECOGNIZER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <filesystem>

namespace HTTP_Server
{
	// Struct to store MIME type and whether it is binary or text
	struct MimeTypeInfo
	{
		std::string mimeType;
		bool is_binary;
	};

	class MimeTypeRecognizer
	{
	public:
		MimeTypeRecognizer();

		MimeTypeInfo getMimeTypeInfo(const std::string &filePath) const;

		void addMimeType(const std::string &extension, const std::string &mimeType, bool is_binary);
		bool isBinaryFile(const std::string &filePath) const;
		std::string getMimeTypeString(const std::string &filePath) const;

	private:
		// Map from file extensions to MIME type information
		std::unordered_map<std::string, MimeTypeInfo> mimeTypeMap;

		std::string getFileExtension(const std::string &filePath) const;
		void initializeMimeTypes();
	};

}

#endif // MIMETYPE_RECOGNIZER_H
