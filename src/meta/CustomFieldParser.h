#pragma once
#include "../utils/Vec2/Vec2.h"
#include "src/utils/Fon/Fon.h"
#include "src/utils/Logging/Logger.h"

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

		static EmptyFieldType parse(const Fon&) {
			// ReSharper disable once CppStaticAssertFailure
			static_assert(false, "FieldParser: Invalid instantiation");
			return {};
		}
	};

	template <fon::FonTypeC T>
	struct FieldParser<T> {
		static constexpr bool enabled = true;

		static std::optional<T> parse(const Fon& j) {
			return j.getIf<T>();
		}
	};

	template <>
	struct FieldParser<vec2> {
		static constexpr bool enabled = true;

		static std::optional<vec2> parse(const Fon& j) {
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

		static std::optional<T> parse(const Fon& j) {
			const auto s = j.getIf<std::string_view>();
			return s ? magic_enum::enum_cast<T>(*s) : std::nullopt;
		}
	};

	template <class T>
	void initField(T& field, const Fon& j, std::string_view key, bool required = false) {
		if (!j.contains(key)) {
			return;
		}
		if (auto opt = FieldParser<T>::parse(j[key])) {
			field = *opt;
		} else if (required) {
			logger.error_and_throw("Required key: ", key);
		}
	}
} // namespace flx::meta
