#pragma once
#include "../utils/File/json.h"
#include "../utils/Vec2/Vec2.h"
#include "src/game/Components/EntityComponents.h"

#include <magic_enum/magic_enum.hpp>

namespace flx::json {
	template <>
	struct FieldParser<vec2> {
		static constexpr bool enabled = true;

		static std::optional<vec2> parse(const Json& j) {
			const auto ret = flx::json::getIf<std::vector<float>>(j);
			if (ret) {
				return vec2{(*ret)[0], (*ret)[1]};
			}
			return {};
		}
	};

	template <class T>
		requires std::is_enum_v<T>
	struct FieldParser<T> {
		static constexpr bool enabled = true;

		static std::optional<T> parse(const Json& j) {
			const auto s = flx::json::getIf<std::string>(j);
			return s ? magic_enum::enum_cast<T>(*s) : std::nullopt;
		}
	};

	template <class T>
	void initField(T& field, const Json& j, std::string_view key) {
		if (!j.contains(key))
			return;
		auto opt = FieldParser<T>::parse(j[key]);
		if (opt) {
			field = *opt;
		}
	}
} // namespace flx::json
