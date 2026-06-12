#pragma once
#include "src/utils/Container/Map.h"
#include "src/utils/Singleton.h"

#include <deque>

class LocTable {
public:
	std::string name;
	Util::StrMap<std::string> contents;
};

N_DEF_SINGLETON(LocManager) {
private:
	Util::StrMap<LocTable> tables;
	std::string lang_ = "";
	N_CONSTEXPR std::string_view defaultLang = "zhs";
	N_CONSTEXPR std::string_view filePath = "./resources/localization/";

public:
	void loadLanguage(std::string_view lang);
	void loadDefaultLanguage();
	std::string debugGetString(std::string_view table, std::string_view entry, bool required = false);
};