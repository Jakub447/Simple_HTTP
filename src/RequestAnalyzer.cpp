#include <iostream>
#include <sstream>
#include <string>
#include "RequestAnalyzer.hpp"

#include "../liblogger/liblogger.hpp"

namespace HTTP_Server
{
	bool RequestAnalyzer::parse_request()
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		size_t headerEndPos = raw_request.find("\r\n\r\n");
		if (headerEndPos == std::string::npos)
		{
			return false; // Invalid request, no separation between headers and body
		}

		// Separate headers and body
		std::string headersPart = raw_request.substr(0, headerEndPos);
		body = raw_request.substr(headerEndPos + 4); // Skip the "\r\n\r\n"

		// Convert headersPart to an istringstream to use with std::getline
		std::istringstream headersStream(headersPart);

		std::string line;
		// Parse the first line (request line)
		if (std::getline(headersStream, line))
		{
			parse_request_line(headersPart);
		}

		// Parse headers using HttpHeaders class
		request_headers.parse_headers(headersPart);

		return true;
	}

	std::vector<std::string> RequestAnalyzer::get_header(const std::string &key) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		return request_headers.get_header(key);
	}

	bool RequestAnalyzer::has_header(const std::string &key) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		return request_headers.has_header(key);
	}

	std::string RequestAnalyzer::get_body() const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		return body;
	}

	// Function to convert string to HTTPMethod enum
	static HTTPMethod parse_method(const std::string &method)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		if (method == "GET")
			return HTTPMethod::GET;
		// if (method == "POST")
		// return HTTPMethod::POST;
		if (method == "PUT")
			return HTTPMethod::PUT;
		// if (method == "DELETE")
		// return HTTPMethod::DELETE;
		return HTTPMethod::UNKNOWN_METHOD;
	}

	// Function to parse the request line (Method, URI, Protocol Version)
	void RequestAnalyzer::parse_request_line(const std::string &line)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		std::istringstream stream(line);
		std::string method_str, version_str;

		stream >> method_str >> req_info.URI >> version_str;

		req_info.method = parse_method(method_str);
		if (version_str == "HTTP/1.0")
			req_info.prot_ver = 10;
		else if (version_str == "HTTP/1.1")
			req_info.prot_ver = 11;
	}

	bool RequestAnalyzer::validate_cache(const std::string& clientETag, const std::string& currentETag) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		return clientETag == currentETag;
	}
}