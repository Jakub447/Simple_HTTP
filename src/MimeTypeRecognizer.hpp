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

		MimeTypeInfo get_mime_type_Info(const std::string &filePath) const;

		void add_mime_type(const std::string &extension, const std::string &mimeType, bool is_binary);
		bool is_binary_file(const std::string &filePath) const;
		std::string get_mime_type_string(const std::string &filePath) const;

	private:
		// Map from file extensions to MIME type information
		std::unordered_map<std::string, MimeTypeInfo> mimeTypeMap;

		std::string get_file_extension(const std::string &filePath) const;
		void initialize_mime_types();
	};

}

#endif // MIMETYPE_RECOGNIZER_H
