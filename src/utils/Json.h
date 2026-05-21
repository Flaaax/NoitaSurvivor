#pragma once
#pragma warning(push)
#pragma warning(disable : 5105)
#pragma warning(disable : 5260)

#include <nlohmann/json.hpp>

#include "Logger.h"
#include "Vec2.h"
#include <fstream>
#include <optional>
#include <type_traits>
#include <vector>

using json = nlohmann::ordered_json;

inline std::fstream openFile(std::string_view filename, std::ios_base::openmode mode = std::ios::in | std::ios::out) {
	std::fstream file(filename.data(), mode);
	if (!file.is_open()) {
		throw std::runtime_error(std::format("can't open file {}", filename));
	}
	return file;
}

inline json readJson(std::string_view filename) {
	json j;
	openFile(filename) >> j;
	return j;
}

inline void readJson(json& j, std::string_view filename) {
	openFile(filename) >> j;
}

template <class T, class U>
T json_parse_or(const json& j, std::string_view key, U&& defaultVal, std::string_view err_info = {}) {
	if (!j.contains(key)) {
		return T(std::forward<U>(defaultVal));
	}
	try {
		if constexpr (std::is_same_v<T, nvec2>) {
			auto val = j[key].get<std::array<float, 2>>();
			return {val[0], val[1]};
		} else {
			return j[key].get<T>();
			;
		}
	} catch (const std::exception& e) {
		Logger::warn("failed to parse json field: {}\ninto type {}\nwith info: {}\nwith error msg: {}", key, typeid(T).name(), err_info, e.what());
		return T(std::forward<U>(defaultVal));
	}
}

template <class T>
inline std::remove_reference_t<T> json_parse_or(const json& j, std::string_view key, T&& defaultVal, std::string_view err_info = {}) {
	return json_parse_or<std::remove_reference_t<T>, T>(j, key, std::forward<T>(defaultVal), err_info);
}

template <class T>
concept JsonType = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;

template <JsonType T>
inline bool is_json_convertible(const json& j) noexcept {
	if constexpr (std::is_same_v<T, std::string_view> || std::is_same_v<T, std::string>) {
		return j.is_string();
	} else if constexpr (std::is_same_v<T, bool>) {
		return j.is_boolean();
	} else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
		return j.is_number();
	}
	return false;
}

inline const json* json_at(const json& j, std::string_view key) {
	return ((j.contains(key)) ? (&j[key]) : nullptr);
}

template <class T>
inline std::optional<T> json_parse(const json& j) noexcept {
	if (is_json_convertible<T>(j)) {
		return j.get<T>();
	}
	return std::nullopt;
}

template <class T>
	requires std::is_same_v<T, std::vector<typename T::value_type>>
inline std::optional<T> json_parse(const json& j) noexcept {
	using ValueType = typename T::value_type;
	if (!j.is_array())
		return std::nullopt;
	std::vector<ValueType> result;
	result.reserve(j.size());
	for (const auto& elem : j) {
		auto opt_elem = json_parse<ValueType>(elem);
		if (!opt_elem)
			return std::nullopt;
		result.push_back(*opt_elem);
	}
	return result;
}

template <class T>
inline std::optional<T> json_parse(const json& j, std::string_view key) noexcept {
	if (auto jj = json_at(j, key)) {
		return json_parse<T>(*jj);
	}
	return std::nullopt;
}

struct EmptyFieldType {
	explicit(false) operator bool() const {
		return false;
	}
};

template <class T>
struct FieldParser {
	static constexpr bool enabled = false;

	static EmptyFieldType parse(const json& j) {
		// ReSharper disable once CppStaticAssertFailure
		static_assert(false, "FieldParser: Invalid instantiation");
		return {};
	}
};

template <JsonType T>
struct FieldParser<T> {
	static constexpr bool enabled = true;

	static std::optional<T> parse(const json& j) {
		return json_parse<T>(j);
	}
};

#pragma warning(pop)
