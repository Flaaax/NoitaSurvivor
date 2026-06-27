#pragma once
#include "src/utils/Container/Map.h"
#include "src/utils/Macro.h"
#include "src/utils/Singleton.h"

namespace flx::app {
	class LocTable {
	public:
		std::string name;
		StrMap<std::string> contents;

		std::string operator[](std::string_view entry) const;
		std::string get(std::string_view entry, bool usePlaceHolder = true) const;
	};

	class LocManager {
	public:
		static void loadLanguage(std::string_view lang);
		static void loadDefaultLanguage();
		static std::string debugGetString(std::string_view table, std::string_view entry, bool usePlaceHolder = true);
		static const LocTable* getTable(std::string_view table, bool required = true);
	};
} // namespace flx::app