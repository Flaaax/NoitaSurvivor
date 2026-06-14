#pragma once

#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"
#include "src/utils/Text/TextRun.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>

namespace flx::ui {
	class NRichTextShape : public sf::Drawable, public sf::Transformable {
	protected:
		const sf::Font* m_font{};
		std::string m_sourceUtf8;

		text::TextStyle m_defaultStyle;
		u32 m_characterSize = 30u;
		u32 m_tabSize = 4u;
		float m_timeSeconds{};
		float m_lineWidth{};

		sf::Color m_outlineColor{};
		float m_outlineThickness{};

		mutable flx::Vector<text::TextRun> m_runs;
		mutable flx::Vector<sf::Vertex> m_vertices;
		mutable flx::Vector<sf::Vertex> m_outlineVertices;
		mutable vec2 m_layoutSize{};

		mutable bool layoutDirty = true;
		mutable bool styleDirty = true;

		void rebuildStyles() const;
		void rebuildVertices() const;

		void appendRun(u64 byteBegin, u64 byteEnd, text::TextStyle style) const;
		nquad applyStyleToQuad(nquad quad, text::TextStyle style, u64 glyphIndex) const;
		static void appendQuad(flx::Vector<sf::Vertex>& vertices, nquad quad, rect textureRect, sf::Color color);
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

		text::TextStyle getDefaultStyle() const;
		void setDefaultStyle(text::TextStyle style);

		vec2 getLayoutSize() const;
		rect getGlobalLayout() const;

		void setOutlineThickness(float thickness);
		void setOutlineColor(sf::Color color);
		// nrect getVisualLayout() const;
	};
} // namespace flx::ui