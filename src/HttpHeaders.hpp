#ifndef HTTPH_HEADERS_H
#define HTTPH_HEADERS_H

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace HTTP_Server
{
	class HTTPHeaders
	{
	public:
		// Add a single header value
		void addHeader(const std::string &key, const std::string &value);

		// Get all values associated with a header key
		std::vector<std::string> getHeader(const std::string &key) const;

		// Check if a header exists
		bool hasHeader(const std::string &key) const;

		// Remove a specific header key and all its associated values
		void removeHeader(const std::string &key);

		void parseHeaders(const std::string &headersString);

		void printAllHeaders() const;

		bool is_connection_closed() const;

		std::string GetAllHeaders() const;

		std::vector<std::pair<std::string, std::string>> get_all_header_pairs() const;

	private:
		std::map<std::string, std::vector<std::string>> headers;
	};
}

#endif // HTTPH_HEADERS_H