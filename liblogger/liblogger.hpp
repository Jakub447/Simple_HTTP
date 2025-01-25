#ifndef LIBLOGGER_H
#define LIBLOGGER_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>

namespace lib_logger
{

// Convenience macro for logging with file and line info
#define LOG(level, message) Logger::Instance().log(level, message, __FILE__, __LINE__)


	enum class LogLevel
		{
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

		void log(LogLevel level, const std::string &message, const char *file, int line);

	private:
		Logger() : log_level_(LogLevel::INFO) {}
		~Logger()
		{
			if (file_stream_.is_open())
				file_stream_.close();
		}

		std::string Format_message(LogLevel level, const std::string &message, const char *file, int line);

		LogLevel log_level_;
		std::ofstream file_stream_;
		std::mutex mtx_;
	};
}
#endif // LIBLOGGER_H