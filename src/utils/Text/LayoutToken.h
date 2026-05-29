#pragma once
#include "TextRun.h"
#include "src/utils/Integers.h"
#include "src/utils/Vec2/Vec2.h"

#include <SFML/Graphics/Font.hpp>

namespace Util::Text {
	inline bool isAsciiLetter(char32_t ch) {
		return (ch >= U'a' && ch <= U'z') || (ch >= U'A' && ch <= U'Z');
	}

	inline bool isAsciiDigit(char32_t ch) {
		return ch >= U'0' && ch <= U'9';
	}

	inline nquad applyItalicOnly(nquad quad) {
		constexpr float shear = 0.22f;
		const float bottom = std::max(quad.lb.y, quad.rb.y);

		quad.lt.x += (bottom - quad.lt.y) * shear;
		quad.rt.x += (bottom - quad.rt.y) * shear;
		quad.lb.x += (bottom - quad.lb.y) * shear;
		quad.rb.x += (bottom - quad.rb.y) * shear;

		return quad;
	}

	struct LayoutToken {
		u64 begin{};
		u64 end{};

		char32_t first{};
		char32_t last{};

		float advance{};
		float right{};
		float minY{};
		float maxY{};
		bool hasBounds{};
	};

	inline std::optional<LayoutToken> getNextToken(const sf::Font& font, const TextRun& run, u64 begin, u32 characterSize, u32 tabSize) {
		const auto& text = run.text;

		if (begin >= text.getSize()) {
			return {};
		}

		const char32_t first = text[begin];
		u64 end = begin + 1u;

		if (isAsciiLetter(first)) {
			while (end < text.getSize() && isAsciiLetter(text[end])) {
				++end;
			}
		} else if (isAsciiDigit(first)) {
			while (end < text.getSize() && isAsciiDigit(text[end])) {
				++end;
			}

			if (end + 1u < text.getSize() && text[end] == U'.' && isAsciiDigit(text[end + 1u])) {
				end += 2u;

				while (end < text.getSize() && isAsciiDigit(text[end])) {
					++end;
				}
			}
		}

		LayoutToken token;
		token.begin = begin;
		token.end = end;
		token.first = first;
		token.last = text[end - 1u];

		if (first == U'\r' || first == U'\n') {
			return token;
		}

		if (first == U'\t') {
			const auto& spaceGlyph = font.getGlyph(U' ', characterSize, false);
			token.advance = spaceGlyph.advance * static_cast<float>(tabSize);
			token.right = token.advance;
			return token;
		}

		float x{};
		char32_t previous{};

		for (u64 i = begin; i < end; ++i) {
			const char32_t ch = text[i];

			if (previous != U'\0') {
				x += font.getKerning(previous, ch, characterSize, false);
			}

			const auto& glyph = font.getGlyph(ch, characterSize, false);
			nquad quad = nquad::fromRect(glyph.bounds);
			quad.offset({x, 0.f});

			if (run.style.effects.has(TextEffect::Italic)) {
				quad = applyItalicOnly(quad);
			}

			if (glyph.bounds.size.x != 0.f && glyph.bounds.size.y != 0.f) {
				token.right = std::max({token.right, quad.lt.x, quad.rt.x, quad.lb.x, quad.rb.x});

				if (!token.hasBounds) {
					token.minY = std::min({quad.lt.y, quad.rt.y, quad.lb.y, quad.rb.y});
					token.maxY = std::max({quad.lt.y, quad.rt.y, quad.lb.y, quad.rb.y});
					token.hasBounds = true;
				} else {
					token.minY = std::min({token.minY, quad.lt.y, quad.rt.y, quad.lb.y, quad.rb.y});
					token.maxY = std::max({token.maxY, quad.lt.y, quad.rt.y, quad.lb.y, quad.rb.y});
				}
			}

			x += glyph.advance;
			previous = ch;
		}

		token.advance = x;
		token.right = std::max(token.right, token.advance);

		return token;
	}
} // namespace Util::Text