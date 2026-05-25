#pragma once
#include "src/utils/Container/Map.h"

#include <deque>

class LocTable {
public:
	std::string name;
	Util::StrMap<std::string> contents;
};

class LocManager {
private:
	Util::StrMap<LocTable> tables;
	std::string lang_ = "";
	N_CONSTEXPR_VAR std::string_view defaultLang = "zhs";
	N_CONSTEXPR_VAR std::string_view filePath = "./resources/localizaion/";

public:
	void loadLanguage(std::string_view lang);
	std::string debugGetString(std::string_view table,std::string_view entry);
};