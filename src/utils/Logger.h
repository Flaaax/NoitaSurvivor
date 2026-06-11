#pragma once
#ifndef _MYLOGGER_H
#define _MYLOGGER_H

#include "Singleton.h"
#include "src/utils/Debug.h"
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _DEBUG
inline constexpr bool n_debug_log = true;
#else
inline constexpr bool n_debug_log = false;
#endif

namespace flx {
	using logptr = std::shared_ptr<spdlog::logger>;

	struct Logger {
		logptr raw{};

		template <typename... Args>
		void info(std::string_view fmt, Args&&... args) {
			raw->info(fmt::runtime(fmt), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void warn(std::string_view fmt, Args&&... args) {
			raw->warn(fmt::runtime(fmt), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void error(std::string_view fmt, Args&&... args) {
			raw->error(fmt::runtime(fmt), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void trace(std::string_view fmt, Args&&... args) {
			raw->trace(fmt::runtime(fmt), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void critical(std::string_view fmt, Args&&... args) {
			raw->critical(fmt::runtime(fmt), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void debug(std::string_view fmt, Args&&... args) {
			if constexpr (n_debug_log) {
				raw->debug(fmt::runtime(fmt), std::forward<Args>(args)...);
			}
		}

		// Throws an error!
		template <typename... Args>
		[[noreturn]] void error_and_throw(std::string_view fmt, Args&&... args) {
			auto err_string = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
			this->error(err_string);
			Util::Debug::printCallStack();
			throw std::runtime_error(err_string);
		}

		static Logger makeAsync(std::string_view logger_id, bool show_id = false) {
			auto _logger = spdlog::stdout_color_mt<spdlog::async_factory>(logger_id.data());
			if (!show_id) {
				_logger->set_pattern("[%H:%M:%S]%^[%l]%$ %v");
			} else {
				_logger->set_pattern("[%n][%H:%M:%S]%^[%l]%$ %v");
			}
			if constexpr (n_debug_log) {
				_logger->set_level(spdlog::level::level_enum::trace);
			}
			return {std::move(_logger)};
		}

		static Logger makeSync(std::string_view logger_id, bool show_id = false) {
			auto _logger = spdlog::stdout_color_st(logger_id.data());
			if (!show_id) {
				_logger->set_pattern("[%H:%M:%S]%^[%l]%$ %v");
			} else {
				_logger->set_pattern("[%n][%H:%M:%S]%^[%l]%$ %v");
			}
			if constexpr (n_debug_log) {
				_logger->set_level(spdlog::level::level_enum::trace);
			}
			return {std::move(_logger)};
		}
	};

	inline Logger logger = Logger::makeAsync("Default");
} // namespace flx

struct LoggerOld {
public:
	template <typename... Args>
	static void info(std::string_view fmt, Args&&... args) {
		flx::logger.info(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void warn(std::string_view fmt, Args&&... args) {
		flx::logger.warn(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void error(std::string_view fmt, Args&&... args) {
		flx::logger.error(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void trace(std::string_view fmt, Args&&... args) {
		flx::logger.trace(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void critical(std::string_view fmt, Args&&... args) {
		flx::logger.critical(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void debug(std::string_view fmt, Args&&... args) {
		flx::logger.debug(fmt, std::forward<Args>(args)...);
	}

	// Throws an error!
	template <typename... Args>
	[[noreturn]] static void error_and_throw(std::string_view fmt, Args&&... args) {
		flx::logger.error_and_throw(fmt, std::forward<Args>(args)...);
	}
};

// class FileLogger : public SharedSingleton<FileLogger> {
//	N_DECL_SINGLETON(FileLogger);
// private:
//	NAsyncLogger m_logger;
//
//	FileLogger() : m_logger(spdlog::rotating_logger_mt("file_logger", "log.txt", 0x100000ULL * 5, 3)) {
//		//5mb per file, total 3 files
//		m_logger->set_pattern("[%Y/%m/%d %T][%l] %v");
//		//m_logger->info("File logger steady.");
//	}
//
//	~FileLogger() {
//		//m_logger->info("File logger closed.");
//	}
//
//	template<typename Level, typename... Args>
//	void log_dual(Level level, const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		m_logger->log(level, fmt, std::forward<Args>(args)...);
//		//Logger::inst().logger()->log(level, fmt, std::forward<Args>(args)...);
//	}
//
// public:
//	template<typename... Args>
//	static void info(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		FileLogger::inst().log_dual(spdlog::level::info, fmt, std::forward<Args>(args)...);
//	}
//
//	template<typename... Args>
//	static void warn(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		FileLogger::inst().log_dual(spdlog::level::warn, fmt, std::forward<Args>(args)...);
//	}
//
//	template<typename... Args>
//	static void error(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		FileLogger::inst().log_dual(spdlog::level::err, fmt, std::forward<Args>(args)...);
//	}
//
//	template<typename... Args>
//	static void trace(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		FileLogger::inst().log_dual(spdlog::level::trace, fmt, std::forward<Args>(args)...);
//	}
//
//	template<typename... Args>
//	static void critical(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		FileLogger::inst().log_dual(spdlog::level::critical, fmt, std::forward<Args>(args)...);
//	}
//
//	template<typename... Args>
//	static void debug(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
//		if constexpr (n_debug_log) {
//			FileLogger::inst().log_dual(spdlog::level::debug, fmt, std::forward<Args>(args)...);
//		}
//	}
//
//	static NAsyncLogger& logger() { return FileLogger::inst().m_logger; }
// };
//
// class FileLogger : public SharedSingleton<FileLogger> {
// 	N_DECL_SINGLETON(FileLogger);
//
// private:
// 	FileLogger() {
// 	}
//
// 	~FileLogger() {
// 	}
//
// 	template <typename Level, typename... Args>
// 	void log_dual(Level level, const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 		// Logger::inst().logger()->log(level, fmt, std::forward<Args>(args)...);
// 	}
//
// public:
// 	template <typename... Args>
// 	static void info(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 	}
//
// 	template <typename... Args>
// 	static void warn(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 	}
//
// 	template <typename... Args>
// 	static void error(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 	}
//
// 	template <typename... Args>
// 	static void trace(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 	}
//
// 	template <typename... Args>
// 	static void critical(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 	}
//
// 	template <typename... Args>
// 	static void debug(const spdlog::format_string_t<Args...>& fmt, Args&&... args) {
// 	}
// };

#endif // _MYLOGGER_H