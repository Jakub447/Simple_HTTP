#include "liblogger.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace lib_logger
{

		static std::string get_date()
	{
		// Get the current time in GMT
		std::time_t now = std::time(nullptr);
		std::tm *gmtTime = std::gmtime(&now);

		// Create a string stream to format the date
		std::ostringstream oss;
		oss << std::put_time(gmtTime, "%a, %d %b %Y %H:%M:%S GMT");

		return oss.str();
	}

	// Sample Implementation of methods
	void Logger::Set_output_file(const std::string &filename)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		if (file_stream_.is_open())
			file_stream_.close();
		file_stream_.open(filename, std::ios::app);
	}

	void Logger::log(LogLevel level, const std::string &message, const char *file, int line)
	{
		if (level < log_level_)
			return;

		std::lock_guard<std::mutex> lock(mtx_);
		std::string formatted_message = Format_message(level, message, file, line);

		// Print to console
		std::cout << formatted_message << std::endl;

		// Print to file if file stream is open
		if (file_stream_.is_open())
		{
			file_stream_ << formatted_message << std::endl;
		}
	}

	std::string Logger::Format_message(LogLevel level, const std::string &message, const char *file, int line)
	{
		std::string formatted_message;
		// Add timestamp, level, etc., to the message
		// ...
		formatted_message = "[" + get_date() + "]" + "(" + "temp_log_level" + ")" + message + "  " + file + std::to_string(line);
		return formatted_message;
	}



}