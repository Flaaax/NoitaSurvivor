#include "Logger.h"

#include "../Debug.h"
#include "ArchiveFileSink.h"
#include "src/utils/Exception/Exceptions.h"

#include <memory>
#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <stdexcept>
#include <string>
#include <utility>

namespace flx {
	void Logger::impl_log(spdlog::level::level_enum level, std::string msg) const {
		raw->log(level, msg);
		if (level == spdlog::level::err) {
			raw->flush();
		}
	}

	void Logger::impl_error_and_throw(std::string msg) {
		flx::debug::printCallStack();
		const std::string msg1 = vformat("{}\n{}", msg, debug::getCallStackStr());
		this->error(msg1);
		throw except::LogThrow(msg1);
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
