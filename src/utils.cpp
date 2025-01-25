#include <fstream>
#include <sstream>

namespace HTTP_Server
{
	std::string concatenate_path(const std::string &rootDir, const std::string &URI)
	{
		// Ensure rootDir ends with a '/'
		std::string formattedRootDir = rootDir;
		if (!formattedRootDir.empty() && formattedRootDir.back() != '/')
		{
			formattedRootDir += '/';
		}

		// Ensure URI starts with a '/'
		std::string formattedURI = URI;
		if (!formattedURI.empty() && formattedURI.front() == '/')
		{
			formattedURI = formattedURI.substr(1); // Remove leading '/'
		}

		// Concatenate the strings
		return formattedRootDir + formattedURI; // Returns the full path
	}

}