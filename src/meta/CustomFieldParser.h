#pragma once
#include "../utils/File/Json.h"
#include "../utils/Vec2/Vec2.h"

#include <magic_enum/magic_enum.hpp>

namespace flx::meta {
	struct EmptyFieldType {
		explicit(false) operator bool() const {
			return false;
		}
	};

	template <class T>
	struct FieldParser {
		static constexpr bool enabled = false;

		static EmptyFieldType parse(const Json&) {
			// ReSharper disable once CppStaticAssertFailure
			static_assert(false, "FieldParser: Invalid instantiation");
			return {};
		}
	};

	template <json::JsonType T>
	struct FieldParser<T> {
		static constexpr bool enabled = true;

		static std::optional<T> parse(const Json& j) {
			return j.getIf<T>();
		}
	};

	template <>
	struct FieldParser<vec2> {
		static constexpr bool enabled = true;

		static std::optional<vec2> parse(const Json& j) {
			if ((!j.isArray()) || j.size() != 2) {
				return {};
			}
			const auto x = j.getIf<float>(0);
			const auto y = j.getIf<float>(1);
			if (x && y) {
				return vec2{*x, *y};
			}
			return {};
		}
	};

	template <class T>
		requires std::is_enum_v<T>
	struct FieldParser<T> {
		static constexpr bool enabled = true;

		static std::optional<T> parse(const Json& j) {
			const auto s = j.getIf<std::string_view>();
			return s ? magic_enum::enum_cast<T>(*s) : std::nullopt;
		}
	};

	template <class T>
	void initField(T& field, const Json& j, std::string_view key) {
		if (!j.contains(key)) {
			return;
		}
		if (auto opt = FieldParser<T>::parse(j[key])) {
			field = *opt;
		}
	}
} // namespace flx::meta
