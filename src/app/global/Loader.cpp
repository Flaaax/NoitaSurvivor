#include "Loader.h"

#include "src/utils/Logging/Logger.h"

namespace flx::app {
	static Logger logger = Logger::makeAsync("Loader");

	void Loader::traverseFolder(const Path& folder, const FileCallback& onFile, std::string entry) {
		namespace fs = std::filesystem;

		if (fs::exists(folder / ignore_file)) {
			return;
		}

		for (auto& entry1 : fs::directory_iterator(folder)) {
			auto path = entry1.path();

			if (entry1.is_regular_file()) {
				onFile(path, entry);
			} else if (entry1.is_directory()) {
				if (!entry.empty()) {
					entry += "/";
				}
				entry += path.string();
				traverseFolder(path, onFile, entry | move);
			}
		}
	}
} // namespace flx::app
