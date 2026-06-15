#include "Loader.h"

#include "src/utils/Logging/Logger.h"

namespace flx::app {
	static Logger logger = Logger::makeAsync("Loader");

	namespace fs = std::filesystem;

	void Loader::traverseFolder(const Path& folder, const FileCallback& onFile) {
		if (exists(folder / Loader::ignore_file))
			return;

		for (auto& item : std::filesystem::directory_iterator(folder)) {
			if (item.is_regular_file()) {
				onFile(item.path());
			} else if (item.is_directory()) {
				traverseFolder(item.path(), onFile);
			}
		}
	}

	std::string Loader::makeEntry(const Path& file, const Path& root) {
		auto relative = fs::proximate(file, root);
		relative.replace_extension();
		return relative.generic_string();
	}
} // namespace flx::app
