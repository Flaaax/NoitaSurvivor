#pragma once
#include"src/utils/Json.h"
#include"EnumMeta.h"
#include"src/utils/Vec2.h"
#include"src/game/Components/EntityComponents.h"

template<>
struct FieldParser<nvec2> {
	static constexpr bool enabled = true;
	static std::optional<nvec2> parse(const json& j) {
		auto ret = json_parse<std::vector<float>>(j);
		if (ret) {
			return nvec2{ (*ret)[0], (*ret)[1] };
		}
		return {};
	}
};

template<>
struct FieldParser<ContactLayer> {
	static constexpr bool enabled = true;
	static std::optional<ContactLayer> parse(const json& j) {
		auto s = json_parse<std::string>(j);
		return s ? EnumMeta::try_get<ContactLayer>("ContactLayer", *s) : std::nullopt;
	}
};

template<class T>
void json_init_field(T& field, const json& j, std::string_view key) {
	if (!j.contains(key))return;
	auto opt = FieldParser<T>::parse(j[key]);
	if (opt) {
		field = *opt;
	}
}