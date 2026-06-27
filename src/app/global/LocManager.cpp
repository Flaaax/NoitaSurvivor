#include "../global/LocManager.h"

#include "../../utils/File/json.h"
#include "src/utils/Container/View.h"
#include "src/utils/Fon/Fon.h"
#include "src/utils/Logging/Logger.h"

namespace flx::app {
	namespace fs = std::filesystem;

	namespace {
		Logger logger = Logger::makeAsync("LocManager");

		FLX_DEF_SINGLETON(Impl) {
		public:
			StrMap<LocTable> tables;
			std::string lang_ = "";
			FLX_CONSTEXPR std::string_view defaultLang = "zhs";
			FLX_CONSTEXPR std::string_view filePath = "./resources/localization/";

			void loadLanguage(std::string_view lang) {
				if (lang == lang_) {
					logger.warn("The language did not change!");
					return;
				}

				logger.info("Loading language: {}", lang);

				tables.clear();

				lang_ = lang;
				const fs::path locPath = fs::path(filePath) / lang_;

				constexpr auto validExtensions = std::to_array({L".json", L".jsonc", L".fon", L".fonx", L".fson"});

				for (const auto& entry : fs::directory_iterator(locPath)) {
					if (!entry.is_regular_file() || !view::all(validExtensions).contains(entry.path().extension())) {
						continue;
					}

					auto category = entry.path().stem();

					auto j = Fon::loadFromFile(entry.path());
					auto& table = tables[category.string()];
					table.name = category.string();

					for (auto& [key, val] : j.items()) {
						if (auto str = val.getIf<std::string>()) {
							table.contents[key] = std::move(*str);
						} else {
							logger.warn("Invalid Loc: {}.{}", category.string(), key);
						}
					}
				}

				logger.info("Load complete. Total {} files.", tables.size());
			}

			void loadDefaultLanguage() {
				loadLanguage(defaultLang);
			}

			std::string debugGetString(std::string_view table, std::string_view entry, bool usePlaceHolder) {
				try {
					return tables.at(table).contents.at(entry);
				} catch (...) {}

				if (usePlaceHolder) {
					logger.warn("Required loc missing: {}.{}", table, entry);
					return std::format("{}.{}", table, entry);
				}
				return {};
			}

			const LocTable* getTable(std::string_view table, bool required) {
				try {
					return &tables.at(table);
				} catch (...) {
					if (required) {
						throw;
					}
				}
				return {};
			}
		};

		auto& inst() {
			return Impl::inst();
		}

	} // namespace

	std::string LocTable::operator[](std::string_view entry) const {
		return get(entry);
	}

	std::string LocTable::get(std::string_view entry, bool usePlaceHolder) const {
		try {
			return contents.at(entry);
		} catch (...) {
		}
		if (usePlaceHolder) {
			logger.warn("Required loc missing: {}.{}", name, entry);
			return std::format("{}.{}", name, entry);
		}
		return {};
	}

	void LocManager::loadLanguage(std::string_view lang) {
		inst().loadLanguage(lang);
	}

	void LocManager::loadDefaultLanguage() {
		inst().loadDefaultLanguage();
	}

	std::string LocManager::debugGetString(std::string_view table, std::string_view entry, bool usePlaceHolder) {
		return inst().debugGetString(table, entry, usePlaceHolder);
	}

	const LocTable* LocManager::getTable(std::string_view table, bool required) {
		return inst().getTable(table, required);
	}
} // namespace flx::app