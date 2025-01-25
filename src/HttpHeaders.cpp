#include "HttpHeaders.hpp"

namespace HTTP_Server
{
	// Trims leading and trailing whitespace from a string
	static void trim(std::string &s)
	{
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
		headers[key].push_back(value);
	}

	// Get all values associated with a header key
	std::vector<std::string> HTTPHeaders::get_header(const std::string &key) const
	{
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
		return headers.find(key) != headers.end();
	}

	// Remove a specific header key and all its associated values
	void HTTPHeaders::remove_header(const std::string &key)
	{
		headers.erase(key);
	}

	void HTTPHeaders::parse_headers(const std::string &headersString)
	{
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

				add_header(key, value);
			}
		}
	}

	// This is your existing function, modified to return a concatenated string
	std::string HTTPHeaders::Get_all_headers() const
	{
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
		for (const auto &[key, values] : headers)
		{
			for (const auto &value : values)
			{
				std::cout << key << ": " << value << std::endl;
			}
		}
	}

	bool HTTPHeaders::is_connection_closed() const
	{
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
	std::vector<std::pair<std::string, std::string>> HTTPHeaders::get_all_header_pairs() const {
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