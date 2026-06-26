#pragma once
#include "../Pointer.h"

#include <memory>
#include <mutex>
#include <spdlog/sinks/sink.h>
#include <string>

namespace flx {
	namespace internal {
		class LogFileWriter;
	}

	class ArchiveFileSink : public spdlog::sinks::sink {
	private:
		Shared<internal::LogFileWriter> writer;
		Unique<spdlog::formatter> formatter;
		std::mutex mutex;

	public:
		explicit ArchiveFileSink();

		void log(const spdlog::details::log_msg& msg) override;
		void flush() override;

		void set_pattern(const std::string& pattern) override;
		void set_formatter(Unique<spdlog::formatter> formatter_) override;
	};
} // namespace flx
