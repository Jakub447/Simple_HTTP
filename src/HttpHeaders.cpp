#include "HttpHeaders.hpp"

#include "../liblogger/liblogger.hpp"

namespace HTTP_Server
{
	// Trims leading and trailing whitespace from a string
	static void trim(std::string &s)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		// Trim leading whitespace
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
										{ return !std::isspace(ch); }));

		// Trim trailing whitespace
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
							 { return !std::isspace(ch); })
					.base(),
				s.end());
	}

	void HTTPHeaders::add_header(const std::string &key, const std::string &value)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		headers[key].push_back(value);
	}

	// Get all values associated with a header key
	std::vector<std::string> HTTPHeaders::get_header(const std::string &key) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		auto it = headers.find(key);
		if (it != headers.end())
		{
			return it->second;
		}
		return {}; // Return an empty vector if key not found
	}

	// Check if a header exists
	bool HTTPHeaders::has_header(const std::string &key) const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		return headers.find(key) != headers.end();
	}

	// Remove a specific header key and all its associated values
	void HTTPHeaders::remove_header(const std::string &key)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		headers.erase(key);
	}

	static bool is_valid_header_name(const std::string &name)
	{
		// Allow only alphanumerics and hyphens (standard HTTP header format)
		return !name.empty() && name.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-") == std::string::npos;
	}

	static bool is_valid_header_value(const std::string &value)
	{
		// Disallow CR and LF characters to prevent header injection
		return value.find('\r') == std::string::npos && value.find('\n') == std::string::npos;
	}

	void HTTPHeaders::parse_headers(const std::string &headersString)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		std::istringstream stream(headersString);
		std::string line;

		while (std::getline(stream, line) && line != "\r")
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

			size_t delimiterPosition = line.find(":");
			if (delimiterPosition != std::string::npos)
			{
				std::string key = line.substr(0, delimiterPosition);
				std::string value = line.substr(delimiterPosition + 1);

				trim(key);
				trim(value);

				// Sanitize header key and value
				if (is_valid_header_name(key) && is_valid_header_value(value))
				{
					add_header(key, value);
				}
				else
				{
					lib_logger::LOG(lib_logger::LogLevel::WARNING, "Invalid header detected, skipping: " + key);
				}
			}
		}
	}

	// This is your existing function, modified to return a concatenated string
	std::string HTTPHeaders::Get_all_headers() const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		std::string result;

		for (const auto &[key, values] : headers)
		{
			for (const auto &value : values)
			{
				result += key + ": " + value + "\r\n";
			}
		}

		// Return the final headers string
		return result;
	}

	void HTTPHeaders::print_all_headers() const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		for (const auto &[key, values] : headers)
		{
			for (const auto &value : values)
			{
				lib_logger::LOG(lib_logger::LogLevel::INFO, "%s:%s", key, value);
			}
		}
	}

	bool HTTPHeaders::is_connection_closed() const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		// Use the get_header method to fetch the values associated with the "Connection" key
		std::vector<std::string> connectionValues = get_header("Connection");

		// Iterate through the values to see if "close" is specified
		for (const auto &value : connectionValues)
		{
			// Convert to lowercase to ensure case-insensitive comparison
			std::string lowerValue = value;
			std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

			if (lowerValue == "close")
			{
				return true; // Connection should be closed
			}
		}

		return false; // Connection is not closed (persistent)
	}

	// Update the Get_all_headers to return a vector of pairs
	std::vector<std::pair<std::string, std::string>> HTTPHeaders::get_all_header_pairs() const
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "");
		std::vector<std::pair<std::string, std::string>> result;
		for (const auto &[key, values] : headers)
		{
			for (const auto &value : values)
			{
				result.emplace_back(key, value); // Store pairs of headers
			}
		}
		return result; // Return vector of pairs
	}

}