#pragma once
#include "src/utils/Pointer.h"

#include <memory>
#include <spdlog/sinks/sink.h>
#include <string>

namespace flx {
	namespace internal {
		class LogFileWriter;
	}

	class ArchiveFileSink : public spdlog::sinks::sink {
	private:
		n_shared<internal::LogFileWriter> writer;
		std::mutex mutex;
		n_unique<spdlog::formatter> formatter;

	public:
		explicit ArchiveFileSink();

		void log(const spdlog::details::log_msg& msg) override;
		void flush() override;

		void set_pattern(const std::string& pattern) override;
		void set_formatter(n_unique<spdlog::formatter> formatter) override;
	};
} // namespace flx