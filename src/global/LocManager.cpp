#include "LocManager.h"

#include "../utils/File/Json.h"

namespace fs = std::filesystem;

void LocManager::loadLanguage(std::string_view lang) {
	if (lang == lang_) {
		Logger::warn("The language did not change!");
		return;
	}

	Logger::info("Loading language: {}", lang);

	tables.clear();

	lang_ = lang;
	const fs::path locPath = fs::path(filePath) / lang_;

	for (const auto& entry : fs::directory_iterator(locPath)) {
		if (!entry.is_regular_file() || entry.path().extension() != ".jsonc") {
			continue;
		}

		auto category = entry.path().stem();

		json j = Util::Json::loadFromFile(entry.path().string());
		auto& table = tables[category.string()];
		table.name = category.string();

		for (auto& [key, val] : j.items()) {
			if (auto str = Util::Json::getIf<std::string>(val)) {
				table.contents[key] = std::move(*str);
			} else {
				Logger::warn("Invalid Loc: {}.{}", category.string(), key);
			}
		}
	}

	Logger::info("Load complete.");
}

void LocManager::loadDefaultLanguage() {
	loadLanguage(defaultLang);
}

std::string LocManager::debugGetString(std::string_view table, std::string_view entry, bool required) {
	try {
		return tables.at(table).contents.at(entry);
	} catch (...) {}

	if (required) {
		Logger::warn("Required loc missing: {}.{}", table, entry);
		return std::format("{}.{}", table, entry);
	}
	return {};
}