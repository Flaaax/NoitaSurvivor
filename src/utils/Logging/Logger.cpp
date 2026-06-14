#include "Logger.h"

#include "ArchiveFileSink.h"
#include "src/utils/Debug.h"
#include "src/utils/Pointer.h"

#include <chrono>
#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace flx {
	static std::string defaultLogFilePath = "logs/flx.log";

	void Logger::_impl_log(spdlog::level::level_enum level, std::string msg) const {
		raw->log(level, msg);
	}

	void Logger::_impl_error_and_throw(std::string msg) {
		flx::Debug::printCallStack();
		this->error(msg);
		throw std::runtime_error(msg);
	}

	Logger Logger::makeAsync(std::string_view logger_id, bool showID) {
		auto _logger = spdlog::stdout_color_mt<spdlog::async_factory>(logger_id.data());
		_logger->sinks().emplace_back(std::make_shared<ArchiveFileSink>());
		setPattern(_logger, showID);
		return {std::move(_logger)};
	}

	Logger Logger::makeSync(std::string_view logger_id, bool showID) {
		auto _logger = spdlog::stdout_color_st(logger_id.data());
		_logger->sinks().emplace_back(std::make_shared<ArchiveFileSink>());
		setPattern(_logger, showID);
		return {std::move(_logger)};
	}

	void Logger::setPattern(logptr logger, bool showID) {
		if (showID) {
			logger->set_pattern("[%H:%M:%S][%n]%^[%l]%$ %v");
		} else {
			logger->set_pattern("[%H:%M:%S]%^[%l]%$ %v");
		}
		if constexpr (n_debug_log) {
			logger->set_level(spdlog::level::level_enum::trace);
		}
	}

} // namespace flx