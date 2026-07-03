#include "Logger.h"

#include "../Debug.h"
#include "../Exception/Exceptions.h"
#include "ArchiveFileSink.h"

#include <memory>
#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <utility>

namespace flx {
	void Logger::impl_log(spdlog::level::level_enum level, std::string_view msg) const {
		raw->log(level, msg);
		if (level == spdlog::level::err) {
			raw->flush();
		}
	}

	void Logger::impl_error_and_throw(std::string msg) {
		// flx::debug::printCallStack();
		const auto msg1 = vformat("{}\n{}", msg, debug::getCallStackStr());
		this->error(msg1);
		throw except::LogThrow(msg1);
	}

	Logger Logger::makeAsync(std::string_view logger_id, bool showID) {
		auto logger_ = spdlog::stdout_color_mt<spdlog::async_factory>(std::string(logger_id));
		logger_->sinks().emplace_back(std::make_shared<ArchiveFileSink>());
		setPattern(logger_, showID);
		return {std::move(logger_)};
	}

	Logger Logger::makeSync(std::string_view logger_id, bool showID) {
		auto logger_ = spdlog::stdout_color_st(std::string(logger_id));
		logger_->sinks().emplace_back(std::make_shared<ArchiveFileSink>());
		setPattern(logger_, showID);
		return {std::move(logger_)};
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
