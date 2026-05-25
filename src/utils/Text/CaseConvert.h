#pragma once
#include <string>

namespace Util {
	constexpr bool is_ascii_upper(char c) {
		return c >= 'A' && c <= 'Z';
	}

	constexpr char ascii_lower(char c) {
		return is_ascii_upper(c) ? static_cast<char>(c - 'A' + 'a') : c;
	}

	inline std::string pascalToSnake(std::string_view text) {
		std::string result;
		result.reserve(text.size());

		for (const char c : text) {
			if (is_ascii_upper(c)) {
				if (!result.empty()) {
					result.push_back('_');
				}

				result.push_back(ascii_lower(c));
			} else {
				result.push_back(c);
			}
		}

		return result;
	}
}