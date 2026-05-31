#pragma once

#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"
#include "src/utils/Text/TextRun.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>

class NRichTextShape : public sf::Drawable, public sf::Transformable {
protected:
	const sf::Font* m_font{};
	std::string m_sourceUtf8;

	Util::TextStyle m_defaultStyle;
	u32 m_characterSize = 30u;
	u32 m_tabSize = 4u;
	float m_timeSeconds{};
	float m_lineWidth{};

	mutable Util::Vector<Util::TextRun> m_runs;
	mutable Util::Vector<sf::Vertex> m_vertices;
	mutable nvec2 m_layoutSize{};

	mutable bool layoutDirty = true;
	mutable bool styleDirty = true;

	void rebuildStyles() const;
	void rebuildVertices() const;

	void appendRun(u64 byteBegin, u64 byteEnd, Util::TextStyle style) const;
	nquad applyStyleToQuad(nquad quad, Util::TextStyle style, u64 glyphIndex) const;
	void appendQuad(nquad quad, nrect textureRect, sf::Color color) const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	explicit NRichTextShape(const sf::Font& font, std::string_view utf8Markup = {}, u32 characterSize = 30u);
	void rebuildCache() const;

	void setFont(const sf::Font& font);
	const sf::Font& getFont() const;

	void setString(std::string_view utf8Markup);
	std::string_view getString() const;

	void setCharacterSize(u32 size);
	u32 getCharacterSize() const;

	void setLineWidth(float width);
	float getLineWidth() const;

	void setTabSize(u32 spaces);
	void setTime(float seconds);

	void setDefaultColor(sf::Color color);

	Util::TextStyle getDefaultStyle() const;
	void setDefaultStyle(Util::TextStyle style);

	nvec2 getLayoutSize() const;
	nrect getGlobalLayout()const;
	// nrect getVisualLayout() const;
};
