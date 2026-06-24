#pragma once
#include "BBCode.h"
#include "src/utils/FlagSet.h"
#include "src/utils/Integers.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/String.hpp>
#include <optional>

namespace flx::text {
	enum class TextEffect : u64 {
		None = 0u,
		Sine = 1u << 0u,
		Shake = 1u << 1u,
		Bold = 1u << 2u,
		Italic = 1u << 3u
	};

	struct TextStyle {
		sf::Color color = sf::Color::Black;
		FlagSet<TextEffect> effects;

		bool operator==(const TextStyle&) const = default;

		bool isBold() const {
			return effects.has(TextEffect::Bold);
		}
	};

	struct TextRun {
		sf::String text;
		TextStyle style;
	};

	inline std::optional<TextStyle> getNextStyle(std::string_view tag, TextStyle style) {
		text::trim(tag);
		if (tag.empty()) {
			return {};
		}

		if (const auto color = text::parseNamedColorTag(tag)) {
			style.color = *color;
			return style;
		}

		if (const auto color = text::parseRgbTag(tag)) {
			style.color = *color;
			return style;
		}

		if (tag == "i") {
			style.effects.add(TextEffect::Italic);
			return style;
		}

		if (tag == "b") {
			style.effects.add(TextEffect::Bold);
			return style;
		}

		if (tag == "sine") {
			style.effects.add(TextEffect::Sine);
			return style;
		}

		if (tag == "shake") {
			style.effects.add(TextEffect::Shake);
			return style;
		}

		return {};
	}
} // namespace Util
