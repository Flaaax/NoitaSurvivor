#pragma once
#pragma warning(push)
#pragma warning(disable : 5105)
#pragma warning(disable : 5260)

#include "../Logging/Logger.h"
#include "../Vec2/Vec2.h"
#include "File.h"
#include "src/utils/Container/Vector.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <type_traits>
#include <vector>

namespace flx::json {
	using Json = nlohmann::ordered_json;

	consteval auto validExtensions() {
		return Array<std::string_view, 2>{".json", ".jsonc"};
	}

	constexpr bool isValidExtension(const std::filesystem::path& path) {
		return validExtensions().view().contains(path);
	}

	inline Json loadFromFile(const std::filesystem::path& file) {
		return Json::parse(file::open(file), {}, true, true);
	}

	template <class T>
	bool isType(const Json& j) {
		if constexpr (std::is_same_v<T, std::string>) {
			return j.is_string();
		} else if constexpr (std::is_same_v<T, bool>) {
			return j.is_boolean();
		} else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
			return j.is_number();
		}
		return false;
	}

	template <class T>
	std::optional<T> getIf(const Json& j) {
		if (!isType<T>(j)) {
			return {};
		}
		return j.get<T>();
	}

	template <class T>
		requires std::is_same_v<T, std::vector<typename T::value_type>>
	std::optional<T> getIf(const Json& j) noexcept {
		using ValueType = T::value_type;
		if (!j.is_array()) {
			return {};
		}
		std::vector<ValueType> result;
		result.reserve(j.size());
		for (const auto& elem : j) {
			auto opt_elem = getIf<ValueType>(elem);
			if (!opt_elem) {
				return {};
			}
			result.emplace_back(std::move(*opt_elem));
		}
		return result;
	}

	template <class T>
	T getOr(const Json& j, const T& defaultVal) {
		if (auto jj = getIf<T>(j)) {
			return *jj;
		}
		return defaultVal;
	}

	template <class T, class U>
	T parseOr(const Json& j, std::string_view key, U&& defaultVal, std::string_view err_info = {}) {
		if (!j.contains(key)) {
			return T(std::forward<U>(defaultVal));
		}
		try {
			if constexpr (std::is_same_v<T, vec2>) {
				auto val = j[key].get<std::array<float, 2>>();
				return {val[0], val[1]};
			} else {
				return j[key].get<T>();
			}
		} catch (const std::exception& e) {
			flx::logger.warn("failed to parse json field: {}\ninto type {}\nwith info: {}\nwith error msg: {}", key, typeid(T).name(), err_info, e.what());
			return T(std::forward<U>(defaultVal));
		}
	}

	template <class T>
	std::remove_reference_t<T> parseOr(const Json& j, std::string_view key, T&& defaultVal, std::string_view err_info = {}) {
		return parseOr<std::remove_reference_t<T>, T>(j, key, std::forward<T>(defaultVal), err_info);
	}

	template <class T>
	concept JsonType = std::is_integral_v<T> ||
					   std::is_floating_point_v<T> ||
					   std::is_same_v<T, std::string> ||
					   std::is_same_v<T, std::string_view>;

	inline const Json* jsonAt(const Json& j, std::string_view key) {
		return ((j.contains(key)) ? (&j[key]) : nullptr);
	}

	template <class T>
	std::optional<T> parse(const Json& j, std::string_view key) noexcept {
		if (const auto jj = jsonAt(j, key)) {
			return flx::json::getIf<T>(*jj);
		}
		return std::nullopt;
	}

	struct EmptyField {
		explicit(false) operator bool() const {
			return false;
		}
	};

	template <class T>
	struct FieldParser {
		static constexpr bool enabled = false;

		static EmptyField parse(const Json&) {
			// ReSharper disable once CppStaticAssertFailure
			static_assert(false, "FieldParser: Invalid instantiation");
			return {};
		}
	};

	template <JsonType T>
	struct FieldParser<T> {
		static constexpr bool enabled = true;

		static std::optional<T> parse(const Json& j) {
			return flx::json::getIf<T>(j);
		}
	};

} // namespace flx::json

namespace flx {
	using json::Json;
}

#pragma warning(pop)
