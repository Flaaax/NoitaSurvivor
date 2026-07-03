#pragma once

#include "../Pointer.h"
#include "../Text/Format.h"

#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include <string>
#include <string_view>
#include <utility>

#ifdef _DEBUG
inline constexpr bool n_debug_log = true;
#else
inline constexpr bool n_debug_log = false;
#endif

namespace spdlog {
	class logger;
}

namespace flx {
	using logptr = Shared<spdlog::logger>;

	class Logger {
	private:
		void impl_log(spdlog::level::level_enum level, std::string_view msg) const;
		[[noreturn]] void impl_error_and_throw(std::string msg);

	public:
		logptr raw{};

		template <typename... Args>
		void info(std::string_view fmt, Args&&... args) {
			this->impl_log(spdlog::level::info, flx::vformat(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void warn(std::string_view fmt, Args&&... args) {
			this->impl_log(spdlog::level::warn, flx::vformat(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void error(std::string_view fmt, Args&&... args) {
			this->impl_log(spdlog::level::err, flx::vformat(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void trace(std::string_view fmt, Args&&... args) {
			this->impl_log(spdlog::level::trace, flx::vformat(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void critical(std::string_view fmt, Args&&... args) {
			this->impl_log(spdlog::level::critical, flx::vformat(fmt, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void debug(std::string_view fmt, Args&&... args) {
			if constexpr (n_debug_log) {
				this->impl_log(spdlog::level::debug, flx::vformat(fmt, std::forward<Args>(args)...));
			}
		}

		// Throws an error!
		template <typename... Args>
		[[noreturn]] void error_and_throw(std::string_view fmt, Args&&... args) {
			auto err_string = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
			this->impl_error_and_throw(std::move(err_string));
		}

		static Logger makeAsync(std::string_view logger_id, bool showID = true);
		static Logger makeSync(std::string_view logger_id, bool showID = true);

		static void setPattern(logptr logger, bool showID);
	};

	inline Logger logger = Logger::makeAsync("flx_logger_global", false);
} // namespace flx

// struct LoggerOld {
// public:
// 	template <typename... Args>
// 	static void info(std::string_view fmt, Args&&... args) {
// 		flx::logger.info(fmt, std::forward<Args>(args)...);
// 	}
//
// 	template <typename... Args>
// 	static void warn(std::string_view fmt, Args&&... args) {
// 		flx::logger.warn(fmt, std::forward<Args>(args)...);
// 	}
//
// 	template <typename... Args>
// 	static void error(std::string_view fmt, Args&&... args) {
// 		flx::logger.error(fmt, std::forward<Args>(args)...);
// 	}
//
// 	template <typename... Args>
// 	static void trace(std::string_view fmt, Args&&... args) {
// 		flx::logger.trace(fmt, std::forward<Args>(args)...);
// 	}
//
// 	template <typename... Args>
// 	static void critical(std::string_view fmt, Args&&... args) {
// 		flx::logger.critical(fmt, std::forward<Args>(args)...);
// 	}
//
// 	template <typename... Args>
// 	static void debug(std::string_view fmt, Args&&... args) {
// 		flx::logger.debug(fmt, std::forward<Args>(args)...);
// 	}
//
// 	// Throws an error!
// 	template <typename... Args>
// 	[[noreturn]] static void error_and_throw(std::string_view fmt, Args&&... args) {
// 		flx::logger.error_and_throw(fmt, std::forward<Args>(args)...);
// 	}
// };
