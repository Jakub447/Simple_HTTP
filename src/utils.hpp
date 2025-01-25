#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace HTTP_Server
{

	std::string concatenate_path(const std::string &rootDir, const std::string &URI);

}

#endif // UTILS_HPP
