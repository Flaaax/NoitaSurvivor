#pragma once
#include "src/utils/Container/Map.h"
#include "src/utils/Macro.h"
#include "src/utils/Singleton.h"

namespace flx::app {
	class LocTable {
	public:
		std::string name;
		StrMap<std::string> contents;
	};

	FLX_DEF_SINGLETON(LocManager) {
	private:
		StrMap<LocTable> tables;
		std::string lang_ = "";
		FLX_CONSTEXPR std::string_view defaultLang = "zhs";
		FLX_CONSTEXPR std::string_view filePath = "./resources/localization/";

	public:
		void loadLanguage(std::string_view lang);
		void loadDefaultLanguage();
		std::string debugGetString(std::string_view table, std::string_view entry, bool required = false);
	};
} // namespace flx::app