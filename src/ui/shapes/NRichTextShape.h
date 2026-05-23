#pragma once

#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"
#include "src/utils/Text/TextRun.h"
#include "src/utils/Vec2.h"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class NRichTextShape : public sf::Drawable, public sf::Transformable {
protected:
	const sf::Font* m_font{};
	std::string m_sourceUtf8;

	Util::TextStyle m_defaultStyle;
	u32 m_characterSize = 30u;
	u32 m_tabSize = 4u;
	float m_timeSeconds = 0.f;

	Util::Vector<Util::TextRun> m_runs;
	Util::Vector<sf::Vertex> m_vertices;
	nrect m_visualBounds{};
	nvec2 m_layoutSize{};

	void rebuildAll();
	void rebuildStyles();
	void rebuildVertices();

	void appendRun(u64 byteBegin, u64 byteEnd, Util::TextStyle style);
	nquad applyStyleToQuad(nquad quad, Util::TextStyle style, u64 glyphIndex) const;
	void appendQuad(nquad quad, sf::IntRect textureRect, sf::Color color);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit NRichTextShape(const sf::Font& font, std::string_view utf8Markup = {}, u32 characterSize = 30u);

	void setFont(const sf::Font& font);
	const sf::Font& getFont() const;

	void setString(std::string_view utf8Markup);
	std::string_view getString() const;

	void setCharacterSize(u32 size);
	u32 getCharacterSize() const;

	void setTabSize(u32 spaces);
	void setTime(float seconds);

	Util::TextStyle getDefaultStyle() const;
	void setDefaultStyle(Util::TextStyle style);

	nvec2 getLayoutSize() const;
	nrect getVisualLayout() const;
};
