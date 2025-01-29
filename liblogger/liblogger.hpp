#ifndef LIBLOGGER_H
#define LIBLOGGER_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdarg>  // For va_list, va_start, va_end


namespace lib_logger
{

	enum class LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	class Logger
	{
	public:
		static Logger &Instance()
		{
			static Logger instance;
			return instance;
		}

		void Set_log_level(LogLevel level) { log_level_ = level; }
		void Set_output_file(const std::string &filename);
		void Set_max_file_size(std::size_t max_size) { max_file_size_ = max_size; }

		void log(LogLevel level, const std::string &message, const char *file, int line, ...);

	private:
		Logger() : log_level_(LogLevel::INFO), max_file_size_(10 * 1024 * 1024) {}
		~Logger()
		{
			if (file_stream_.is_open())
				file_stream_.close();
		}

		std::string Format_message(LogLevel level, const std::string &message, const char *file, int line, va_list args);
		void Rotate_log_file();

		LogLevel log_level_;
		std::ofstream file_stream_;
		std::mutex mtx_;
		std::string log_file_;
		std::size_t max_file_size_;

	};

	// Convenience macro for logging with file and line info
	#define LOG(level, message, ...) Logger::Instance().log(level, message, __FUNCTION__, __LINE__, ##__VA_ARGS__)
}
#endif // LIBLOGGER_H