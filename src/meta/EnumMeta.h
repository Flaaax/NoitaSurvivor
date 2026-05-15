#pragma once
#include "../utils/Container/Container.h"
#include "src/utils/Singleton.h"
#include <optional>
#include <string>
#include <unordered_map>

class EnumMeta {
	N_DECL_SINGLETON(EnumMeta);
private:
	template<class T>
	using Map = Util::StdMap<T>;
	using enum_t = long long;

	Map<Map<enum_t>> map;

	EnumMeta();
	void addEnum(std::string_view name, std::initializer_list<std::string_view> list);
public:

	static std::optional<enum_t> try_get(std::string_view name, std::string_view enumName);

	template<class T>
	static std::optional<T> try_get(std::string_view name, std::string_view enumName) {
		auto ret = try_get(name, enumName);
		if (!ret)return std::nullopt;
		return static_cast<T>(ret.value());
	}

	template<class T>
	static T get(std::string_view name, std::string_view enumName) {
		return try_get<T>(name, enumName).value();
	}
};