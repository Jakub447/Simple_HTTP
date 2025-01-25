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
#include <array>

namespace lib_logger
{

	void Logger::Rotate_log_file()
	{
		// Check file size and rotate if needed
		if (file_stream_.tellp() >= max_file_size_)
		{
			file_stream_.close();

			// Generate a timestamp for the rotated file
			std::time_t now = std::time(nullptr);
			std::tm *tm = std::localtime(&now);
			std::ostringstream oss;
			oss << std::put_time(tm, "%Y%m%d%H%M%S");
			std::string rotated_filename = log_file_ + "." + oss.str();

			// Rename the current log file to the rotated file name
			std::rename(log_file_.c_str(), rotated_filename.c_str());

			// Reopen the original log file for appending
			file_stream_.open(log_file_, std::ios::app);
		}
	}

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

		// Check if the log file needs rotation
		Rotate_log_file();

		std::string formatted_message = Format_message(level, message, file, line);

		// Print to console
		std::cout << formatted_message << std::endl;

		// Open log file if not open, or append to the file
		if (!file_stream_.is_open()){
			file_stream_.open(log_file_, std::ios::app);
		}
		file_stream_ << formatted_message << std::endl;

		// Print to file if file stream is open
		// if (file_stream_.is_open())
		//{
		//	file_stream_ << formatted_message << std::endl;
		//}
	}

	static std::string log_level_to_string(LogLevel level)
	{
		constexpr std::array<const char *, 6> log_level_strings{"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};

		auto index = static_cast<size_t>(level);
		if (index < log_level_strings.size())
		{
			return log_level_strings[index];
		}
		return "UNKNOWN";
	}

	static std::string log_level_to_color(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::TRACE:
			return "\033[37m"; // White
		case LogLevel::DEBUG:
			return "\033[36m"; // Cyan
		case LogLevel::INFO:
			return "\033[32m"; // Green
		case LogLevel::WARNING:
			return "\033[33m"; // Yellow
		case LogLevel::ERROR:
			return "\033[31m"; // Red
		case LogLevel::CRITICAL:
			return "\033[1;31m"; // Bright Red
		default:
			return "\033[0m"; // Reset
		}
	}

	std::string Logger::Format_message(LogLevel level, const std::string &message, const char *file, int line)
	{
		std::string formatted_message;
		// Add timestamp, level, etc., to the message
		// ...

		std::string file_to_print = "";
		if (level > LogLevel::DEBUG)
		{
			formatted_message = log_level_to_color(level) + "[*" + log_level_to_string(level) + "*]" + "[" + get_date() + "]" + message + "\033[0m";
		}
		else
		{
			formatted_message = log_level_to_color(level) + "[*" + log_level_to_string(level) + "*]" + "[" + get_date() + "]" + message + " ***" + file + " " + std::to_string(line) + "***" + "\033[0m";
		}

		return formatted_message;
	}

}