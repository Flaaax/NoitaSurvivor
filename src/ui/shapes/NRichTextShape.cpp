#include "NRichTextShape.h"

#include "src/utils/Random.h"
#include "src/utils/Text/LayoutToken.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <string_view>
#include <utility>

NRichTextShape::NRichTextShape(const sf::Font& font, std::string_view utf8Markup, u32 characterSize) : m_font(&font),
																									   m_sourceUtf8(utf8Markup),
																									   m_characterSize(characterSize) {
}

void NRichTextShape::setFont(const sf::Font& font) {
	m_font = &font;
	styleDirty = true;
}

void NRichTextShape::setString(std::string_view utf8Markup) {
	m_sourceUtf8.assign(utf8Markup.begin(), utf8Markup.end());
	styleDirty = true;
}

void NRichTextShape::setCharacterSize(u32 size) {
	m_characterSize = size;
	layoutDirty = true;
}

void NRichTextShape::setTabSize(u32 spaces) {
	m_tabSize = std::max(1u, spaces);
	layoutDirty = true;
}

void NRichTextShape::setTime(float seconds) {
	m_timeSeconds = seconds;
	layoutDirty = true;
}

const sf::Font& NRichTextShape::getFont() const {
	return *m_font;
}

std::string_view NRichTextShape::getString() const {
	return m_sourceUtf8;
}

u32 NRichTextShape::getCharacterSize() const {
	return m_characterSize;
}

void NRichTextShape::setLineWidth(float width) {
	if (width == m_lineWidth) {
		return;
	}
	m_lineWidth = width;
	layoutDirty = true;
}

float NRichTextShape::getLineWidth() const {
	return m_lineWidth;
}

Util::TextStyle NRichTextShape::getDefaultStyle() const {
	return m_defaultStyle;
}

void NRichTextShape::setDefaultStyle(Util::TextStyle style) {
	m_defaultStyle = style;
	styleDirty = true;
}

nvec2 NRichTextShape::getLayoutSize() const {
	rebuildCache();
	return m_layoutSize;
}

// nrect NRichTextShape::getVisualLayout() const {
// 	return getTransform().transformRect(m_visualBounds);
// }

void NRichTextShape::rebuildCache() const {
	if (styleDirty) {
		rebuildStyles();
		rebuildVertices();
	} else if (layoutDirty) {
		rebuildVertices();
	}
	styleDirty = false;
	layoutDirty = false;
}

void NRichTextShape::rebuildStyles() const {
	m_runs.clear();

	if (m_sourceUtf8.empty()) {
		return;
	}

	std::vector<Util::TextStyle> stack;
	stack.push_back(m_defaultStyle);

	const std::string_view input = m_sourceUtf8;

	u64 textBegin = 0u;
	u64 i = 0u;

	while (i < input.size()) {
		// 遍历直到找到标签
		if (input[i] != '[') {
			++i;
			continue;
		}

		// 找到关闭标签
		const auto close = input.find(']', i);

		if (close == std::string_view::npos) {
			break;
		}

		const std::string_view rawTag = input.substr(i + 1u, close - i - 1u);
		bool accepted{}; // 是否为有效标签

		// 如果是关闭标签
		if (rawTag == "/") {
			if (stack.size() > 1u) {
				appendRun(textBegin, i, stack.back());

				stack.pop_back();
				accepted = true;
			}
		}
		// 如果是开启标签
		else if (!rawTag.starts_with('/')) {
			if (auto style = Util::getNextStyle(rawTag, stack.back())) {
				appendRun(textBegin, i, stack.back());

				stack.push_back(*style);
				accepted = true;
			}
		}

		if (accepted) {
			i = close + 1u;
			textBegin = i;
			continue;
		}

		i = close + 1u;
	}

	appendRun(textBegin, input.size(), stack.back());
}

void NRichTextShape::appendRun(u64 byteBegin, u64 byteEnd, Util::TextStyle style) const {
	if (byteBegin >= byteEnd) {
		return;
	}

	const auto begin = m_sourceUtf8.begin() + static_cast<std::ptrdiff_t>(byteBegin);
	const auto end = m_sourceUtf8.begin() + static_cast<std::ptrdiff_t>(byteEnd);
	auto text = sf::String::fromUtf8(begin, end);

	if (!m_runs.empty() && m_runs.back().style == style) {
		m_runs.back().text += text;
		return;
	}

	m_runs.push_back(Util::TextRun{.text = std::move(text), .style = style});
}

void NRichTextShape::rebuildVertices() const {
	using namespace Util::Text;
	m_vertices.clear();
	m_layoutSize = {};

	if (!m_font || m_sourceUtf8.empty() || m_runs.empty()) {
		return;
	}

	const float lineSpacing = m_font->getLineSpacing(m_characterSize);

	u64 glyphCapacity = 0u;
	for (const auto& [text, style] : m_runs) {
		glyphCapacity += text.getSize();
	}
	m_vertices.reserve(glyphCapacity * 6u);

	float layoutX{};
	float layoutY{};
	float lineRight{};
	float layoutMaxX{};

	char32_t layoutPrevious = U'\0';
	bool lineHasToken{};

	constexpr float floatMax = std::numeric_limits<float>::max();
	constexpr float floatMin = std::numeric_limits<float>::lowest();

	float layoutMinY = floatMax;
	float layoutMaxY = floatMin;
	bool hasLayoutBounds{};

	u64 glyphIndex{};

	auto finishLayoutLine = [&] {
		layoutMaxX = std::max(layoutMaxX, lineRight);
	};

	auto newLayoutLine = [&] {
		finishLayoutLine();

		layoutX = 0.f;
		layoutY += lineSpacing;
		lineRight = 0.f;
		layoutPrevious = U'\0';
		lineHasToken = false;
	};

	auto includeTokenY = [&](const LayoutToken& token) {
		if (!token.hasBounds) {
			return;
		}

		layoutMinY = std::min(layoutMinY, layoutY + token.minY);
		layoutMaxY = std::max(layoutMaxY, layoutY + token.maxY);
		hasLayoutBounds = true;
	};

	auto appendToken = [&](const Util::TextRun& run, const LayoutToken& token, float baseX) {
		const auto& text = run.text;
		float x = baseX;
		char32_t previous = U'\0';

		for (u64 i = token.begin; i < token.end; ++i) {
			const char32_t ch = text[i];

			if (ch == U'\r' || ch == U'\n' || ch == U'\t') {
				continue;
			}

			if (previous != U'\0') {
				x += m_font->getKerning(previous, ch, m_characterSize, false);
			}

			const auto& drawGlyph = m_font->getGlyph(ch, m_characterSize, run.style.isBold());
			const nrect bounds = drawGlyph.bounds;

			if (bounds.size.x != 0.f && bounds.size.y != 0.f) {
				nquad quad = nquad::fromRect(bounds);
				quad.offset({x, layoutY});
				quad = applyStyleToQuad(quad, run.style, glyphIndex);
				appendQuad(quad, nrect(drawGlyph.textureRect), run.style.color);
			}

			const auto& layoutGlyph = m_font->getGlyph(ch, m_characterSize, false);
			x += layoutGlyph.advance;

			previous = ch;
			++glyphIndex;
		}
	};

	for (const auto& run : m_runs) {
		u64 i = 0u;

		while (const auto token = getNextToken(*m_font, run, i, m_characterSize, m_tabSize)) {
			i = token->end;

			if (token->first == U'\r') {
				continue;
			}

			if (token->first == U'\n') {
				newLayoutLine();
				continue;
			}

			float leadingKerning{};

			if (layoutPrevious != U'\0' && token->first != U'\t') {
				leadingKerning = m_font->getKerning(layoutPrevious, token->first, m_characterSize, false);
			}

			if (m_lineWidth > 0.f && lineHasToken && layoutX + leadingKerning + token->right > m_lineWidth) {
				newLayoutLine();
				leadingKerning = 0.f;
			}

			layoutX += leadingKerning;

			if (token->first == U'\t') {
				layoutX += token->advance;
				lineRight = std::max(lineRight, layoutX);
				layoutPrevious = U'\0';
				lineHasToken = true;
				continue;
			}

			appendToken(run, *token, layoutX);
			includeTokenY(*token);

			lineRight = std::max(lineRight, layoutX + token->right);
			layoutX += token->advance;
			layoutPrevious = token->last;
			lineHasToken = true;
		}
	}

	finishLayoutLine();

	float vertexYOffset{};

	if (hasLayoutBounds) {
		vertexYOffset = -layoutMinY;
		m_layoutSize = {layoutMaxX, layoutMaxY - layoutMinY};
	} else {
		vertexYOffset = static_cast<float>(m_characterSize);
		m_layoutSize = {layoutMaxX, static_cast<float>(m_characterSize)};
	}

	for (auto& vertex : m_vertices) {
		vertex.position.y += vertexYOffset;
	}
}

nquad NRichTextShape::applyStyleToQuad(nquad quad, Util::TextStyle style, u64 glyphIndex) const {
	nquad ret = quad;
	if (style.effects.has(Util::TextEffect::Italic)) {
		constexpr float shear = 0.22f;
		const float bottom = std::max(ret.lb.y, ret.rb.y);

		ret.lt.x += (bottom - ret.lt.y) * shear;
		ret.rt.x += (bottom - ret.rt.y) * shear;
		ret.lb.x += (bottom - ret.lb.y) * shear;
		ret.rb.x += (bottom - ret.rb.y) * shear;
	}

	nvec2 offset{};

	if (style.effects.has(Util::TextEffect::Sine)) {
		offset.y += std::sin(m_timeSeconds * 6.f + static_cast<float>(glyphIndex) * 0.65f) * 4.f;
	}

	if (style.effects.has(Util::TextEffect::Shake)) {
		const auto tick = static_cast<u32>(std::floor(m_timeSeconds * 30.f));
		const auto index = static_cast<u32>(glyphIndex);

		offset.x += Util::pseudoRandomSigned(index * 73856093u ^ tick * 19349663u) * 2.f;
		offset.y += Util::pseudoRandomSigned(index * 83492791u ^ tick * 2971215073u) * 2.f;
	}

	ret.offset(offset);
	return ret;
}

void NRichTextShape::appendQuad(nquad quad, nrect textureRect, sf::Color color) const {
	const float u1 = textureRect.left();
	const float v1 = textureRect.top();
	const float u2 = textureRect.right();
	const float v2 = textureRect.bottom();

	m_vertices.push_back(sf::Vertex{quad.lt, color, {u1, v1}});
	m_vertices.push_back(sf::Vertex{quad.lb, color, {u1, v2}});
	m_vertices.push_back(sf::Vertex{quad.rb, color, {u2, v2}});

	m_vertices.push_back(sf::Vertex{quad.lt, color, {u1, v1}});
	m_vertices.push_back(sf::Vertex{quad.rb, color, {u2, v2}});
	m_vertices.push_back(sf::Vertex{quad.rt, color, {u2, v1}});
}

void NRichTextShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (!m_font || m_vertices.empty()) {
		return;
	}

	rebuildCache();

	states.transform *= getTransform();
	states.texture = &m_font->getTexture(m_characterSize);
	target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Triangles, states);
}
