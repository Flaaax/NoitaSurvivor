#pragma once
#include <string>

namespace flx::text {
	constexpr bool isASCIIUpper(char c) {
		return c >= 'A' && c <= 'Z';
	}

	constexpr char ASCIILower(char c) {
		return isASCIIUpper(c) ? static_cast<char>(c - 'A' + 'a') : c;
	}

	inline std::string pascalToSnake(std::string_view text) {
		std::string result;
		result.reserve(text.size());

		for (const char c : text) {
			if (isASCIIUpper(c)) {
				if (!result.empty()) {
					result.push_back('_');
				}

				result.push_back(ASCIILower(c));
			} else {
				result.push_back(c);
			}
		}

		return result;
	}
} // namespace flx::text