#include "ArchiveFileSink.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"
#include "src/utils/Singleton.h"

#include <filesystem>
#include <fstream>
#include <mutex>
#include <spdlog/pattern_formatter.h>

namespace flx {
	namespace fs = std::filesystem;

	namespace internal {
		class LogFileWriter {
		private:
			static constexpr std::string_view defaultFilePath = "logs/flx.log";
			static constexpr u64 defaultMaxArchives = 5;

			inline static n_shared<LogFileWriter> inst_{};
			inline static std::mutex mutex;

			const fs::path filePath{};
			const fs::path dir{};
			const u64 maxArchives{};
			std::ofstream file;

			static std::string getTimeString() {
				using namespace std::chrono;
				const auto now = system_clock::now();
				const auto time = system_clock::to_time_t(now);

				std::tm tm{};

				localtime_s(&tm, &time);

				char buffer[32]{};
				std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &tm);
				return buffer;
			}

			Util::Vector<std::filesystem::path> findArchives() const {
				Util::Vector<std::filesystem::path> archives;

				auto dir = this->dir;
				const auto stem = filePath.stem().string();
				const auto ext = filePath.extension().string();
				const auto prefix = stem + "_";

				if (dir.empty()) {
					dir = ".";
				}

				if (!std::filesystem::exists(dir)) {
					return archives;
				}

				for (const auto& entry : std::filesystem::directory_iterator(dir)) {
					if (!entry.is_regular_file()) {
						continue;
					}

					auto path = entry.path();
					auto filename = path.filename().string();

					if (filename.starts_with(prefix) && path.extension() == ext) {
						archives.push_back(path);
					}
				}

				return archives;
			}

			void shrinkArchives() const {
				auto archives = findArchives();

				std::ranges::sort(archives, [](const fs::path& a, const fs::path& b) {
					return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
				});

				for (u64 i = maxArchives; i < archives.size(); ++i) {
					std::error_code ec;
					std::filesystem::remove(archives[i], ec);
				}
			}

			void archiveLastLog() const {
				if (!std::filesystem::exists(filePath)) {
					return;
				}

				if (std::filesystem::is_regular_file(filePath) && std::filesystem::file_size(filePath) == 0) {
					std::filesystem::remove(filePath);
					// There should be a warning...
					return;
				}

				const auto stem = filePath.stem().string();
				const auto extension = filePath.extension().string();

				auto path = dir / (stem + "_" + getTimeString());
				path += extension;

				std::filesystem::rename(filePath, path);
			}

		public:
			// Do not create manually
			explicit LogFileWriter(fs::path filePath, u64 maxArchives)
				: filePath(filePath),
				  dir(filePath.parent_path()),
				  maxArchives(maxArchives) {

				if (!dir.empty()) {
					std::filesystem::create_directories(dir);
				}

				archiveLastLog();
				shrinkArchives();

				file.open(filePath, std::ios::out | std::ios::trunc | std::ios::binary | std::ios::trunc);

				if (!file) {
					throw spdlog::spdlog_ex("Failed to open log file: " + filePath.string());
				}
			}

			static n_shared<LogFileWriter> inst() {
				std::scoped_lock lock(mutex);
				if (!inst_) {
					inst_ = std::make_shared<LogFileWriter>(defaultFilePath, defaultMaxArchives);
				}
				return inst_;
			}

			static bool hasInstance() {
				std::scoped_lock lock(mutex);
				return inst_.get() != nullptr;
			}

			void write(const spdlog::memory_buf_t& buffer) {
				std::scoped_lock lock(mutex);

				file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));

				if (!file) {
					throw spdlog::spdlog_ex("Failed to write log file: " + filePath.string());
				}
			}

			void flush() {
				std::scoped_lock lock(mutex);
				file.flush();
			}

			std::filesystem::path getFilePath() const {
				return filePath;
			}

			u64 getMaxArchives() const {
				return maxArchives;
			}
		};
	} // namespace internal

	ArchiveFileSink::ArchiveFileSink()
		: writer(internal::LogFileWriter::inst()),
		  formatter(std::make_unique<spdlog::pattern_formatter>()) {
	}

	void ArchiveFileSink::log(const spdlog::details::log_msg& msg) {
		spdlog::memory_buf_t formatted;

		{
			std::scoped_lock lock(mutex);
			formatter->format(msg, formatted);
		}

		writer->write(formatted);
	}

	void ArchiveFileSink::flush() {
		writer->flush();
	}

	void ArchiveFileSink::set_pattern(const std::string& pattern) {
		std::scoped_lock lock(mutex);
		formatter = std::make_unique<spdlog::pattern_formatter>(pattern);
	}

	void ArchiveFileSink::set_formatter(n_unique<spdlog::formatter> formatter) {
		std::scoped_lock lock(mutex);
		this->formatter = std::move(formatter);
	}
} // namespace flx