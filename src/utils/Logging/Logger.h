#pragma once
#ifndef _MYLOGGER_H
#define _MYLOGGER_H

#include "src/utils/Text/Format.h"

#ifdef _DEBUG
inline constexpr bool n_debug_log = true;
#else
inline constexpr bool n_debug_log = false;
#endif

namespace spdlog {
	class logger;
}

namespace flx {
	using logptr = std::shared_ptr<spdlog::logger>;

	struct Logger {
		logptr raw{};

		void _impl_log(spdlog::level::level_enum level, std::string msg) const;
		[[noreturn]] void _impl_error_and_throw(std::string msg);

		template <typename... Args>
		void info(std::string_view fmt, Args&&... args) {
			this->_impl_log(spdlog::level::info, Util::format(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void warn(std::string_view fmt, Args&&... args) {
			this->_impl_log(spdlog::level::warn, Util::format(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void error(std::string_view fmt, Args&&... args) {
			this->_impl_log(spdlog::level::err, Util::format(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void trace(std::string_view fmt, Args&&... args) {
			this->_impl_log(spdlog::level::trace, Util::format(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void critical(std::string_view fmt, Args&&... args) {
			this->_impl_log(spdlog::level::critical, Util::format(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void debug(std::string_view fmt, Args&&... args) {
			if constexpr (n_debug_log) {
				this->_impl_log(spdlog::level::debug, Util::format(fmt, std::forward<Args>(args)...));
			}
		}

		// Throws an error!
		template <typename... Args>
		[[noreturn]] void error_and_throw(std::string_view fmt, Args&&... args) {
			auto err_string = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
			this->_impl_error_and_throw(std::move(err_string));
		}

		static Logger makeAsync(std::string_view logger_id, bool showID = false);
		static Logger makeSync(std::string_view logger_id, bool showID = false);

		static void setPattern(logptr logger, bool showID);
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

#endif // _MYLOGGER_H