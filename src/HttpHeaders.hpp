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
		void add_header(const std::string &key, const std::string &value);

		// Get all values associated with a header key
		std::vector<std::string> get_header(const std::string &key) const;

		// Check if a header exists
		bool has_header(const std::string &key) const;

		// Remove a specific header key and all its associated values
		void remove_header(const std::string &key);

		void parse_headers(const std::string &headersString);

		void print_all_headers() const;

		bool is_connection_closed() const;

		std::string Get_all_headers() const;

		std::vector<std::pair<std::string, std::string>> get_all_header_pairs() const;

	private:
		std::map<std::string, std::vector<std::string>> headers;
	};
}

#endif // HTTPH_HEADERS_H