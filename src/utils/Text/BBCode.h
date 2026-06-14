#pragma once
#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"

#include <SFML/Graphics/Color.hpp>
#include <charconv>
#include <optional>
#include <string_view>

namespace sf {
	class Color;
}

namespace flx::text {
	inline void trim(std::string_view& text) {
		constexpr auto isSpace = [](char c) {
			const auto byte = static_cast<unsigned char>(c);
			return byte == ' ' || byte == '\t' || byte == '\n' || byte == '\r' || byte == '\f' || byte == '\v';
		};

		while (!text.empty() && isSpace(text.front())) {
			text.remove_prefix(1u);
		}

		while (!text.empty() && isSpace(text.back())) {
			text.remove_suffix(1u);
		}
	}

	inline std::optional<sf::Color> parseNamedColorTag(std::string_view tag) {
		if (tag == "white") {
			return sf::Color::White;
		}
		if (tag == "black") {
			return sf::Color::Black;
		}
		if (tag == "red") {
			return sf::Color::Red;
		}
		if (tag == "green") {
			return sf::Color::Green;
		}
		if (tag == "blue") {
			return sf::Color::Blue;
		}
		if (tag == "yellow") {
			return sf::Color::Yellow;
		}
		if (tag == "cyan") {
			return sf::Color::Cyan;
		}
		if (tag == "magenta") {
			return sf::Color::Magenta;
		}
		if (tag == "orange") {
			return sf::Color{255u, 165u, 0u};
		}
		return std::nullopt;
	}

	inline std::optional<u8> tryParseU8(std::string_view text) {
		trim(text);
		if (text.empty()) {
			return {};
		}
		i32 value = 0;
		const auto* begin = text.data();
		const auto* end = begin + text.size();
		const auto [ptr, ec] = std::from_chars(begin, end, value);
		if (ec != std::errc{} || ptr != end || value < 0 || value > 255) {
			return {};
		}
		return static_cast<u8>(value);
	}

	inline std::optional<sf::Color> parseRgbTag(std::string_view tag) {
		constexpr std::string_view prefix = "rgb(";

		if (!tag.starts_with(prefix) || !tag.ends_with(')')) {
			return {};
		}

		tag.remove_prefix(prefix.size());
		tag.remove_suffix(1u);

		Array<u8, 3u> values{};
		u64 start = 0u;

		for (const u64 i : values.indices()) {
			const auto comma = tag.find(',', start);
			auto part = tag.substr(start, comma == std::string_view::npos
											  ? std::string_view::npos
											  : comma - start);
			trim(part);
			const auto value = tryParseU8(part);

			if (!value) {
				return {};
			}

			values[i] = *value;

			if (i + 1u < values.size()) {
				if (comma == std::string_view::npos) {
					return {};
				}
				start = comma + 1u;
			} else if (comma != std::string_view::npos) {
				return {};
			}
		}

		return sf::Color{values[0], values[1], values[2]};
	}
} // namespace Util::Text