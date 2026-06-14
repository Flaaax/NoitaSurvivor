#include "../global/LocManager.h"

#include "../../utils/File/json.h"

namespace flx::app {
	namespace fs = std::filesystem;

	void LocManager::loadLanguage(std::string_view lang) {
		if (lang == lang_) {
			logger.warn("The language did not change!");
			return;
		}

		logger.info("Loading language: {}", lang);

		tables.clear();

		lang_ = lang;
		const fs::path locPath = fs::path(filePath) / lang_;

		for (const auto& entry : fs::directory_iterator(locPath)) {
			if (!entry.is_regular_file() || entry.path().extension() != ".jsonc") {
				continue;
			}

			auto category = entry.path().stem();

			Json j = flx::json::loadFromFile(entry.path().string());
			auto& table = tables[category.string()];
			table.name = category.string();

			for (auto& [key, val] : j.items()) {
				if (auto str = flx::json::getIf<std::string>(val)) {
					table.contents[key] = std::move(*str);
				} else {
					logger.warn("Invalid Loc: {}.{}", category.string(), key);
				}
			}
		}

		logger.info("Load complete.");
	}

	void LocManager::loadDefaultLanguage() {
		loadLanguage(defaultLang);
	}

	std::string LocManager::debugGetString(std::string_view table, std::string_view entry, bool required) {
		try {
			return tables.at(table).contents.at(entry);
		} catch (...) {}

		if (required) {
			logger.warn("Required loc missing: {}.{}", table, entry);
			return std::format("{}.{}", table, entry);
		}
		return {};
	}
} // namespace flx::app