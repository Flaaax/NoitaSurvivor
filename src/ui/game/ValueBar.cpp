#include "ValueBar.h"
#include "src/global/AssetManager.h"
#include "src/utils/NString.h"

ValueBar::ValueBar(const nvec2& topRight, const nvec2& size, int initialMaxHealth, float lengthPerHealth, int mode)
	: m_initialHealth(initialMaxHealth),
	  m_initialLength(size.x),
	  lengthPerHealth(lengthPerHealth),
	  m_right(topRight.x),
	  mode(mode), text(AssetMgr::getDefaultFont()) {
	setHealth(initialMaxHealth);
	setMaxHealth(initialMaxHealth);
	geometry.size = size;
	geometry.setTopRight(topRight);

	text.setFillColor({240, 240, 240});
	text.setOutlineColor({0, 0, 0});
	text.setCharacterSize(static_cast<unsigned int>(size.y * 0.9));

	backGroundShape.setFillColor(sf::Color::Black);
	backGroundShape.setRadius(6);
	healthBarShape.setFillColor({100, 100, 100});
	if (mode == 0) {
		healthShape.setFillColor({210, 0, 0});
	} else if (mode == 1) {
		healthShape.setFillColor({0, 210, 0});
	}
	text.setOutlineThickness(1.f);
	text.setStyle(sf::Text::Bold);
}

void ValueBar::updateText() {
	if (mode == 1) {
		text.setString(NString("等级{}", level));
	} else if (mode == 0) {
		text.setString(NString("{} / {}", m_health, m_maxHealth));
	}
}

void ValueBar::draw(const NCanvas& canvas) const {
	auto& renderPos = geometry.position;
	const fvec2 healthShapeSize = {(m_health / static_cast<float>(m_maxHealth)) * geometry.w, geometry.h};
	healthShape.setSize(healthShapeSize);
	healthShape.setPosition({renderPos.x + geometry.w - healthShapeSize.x, renderPos.y});

	nrect posRect = {renderPos, geometry.size};

	healthBarShape.setSize(geometry.size);

	healthBarShape.setPosition(posRect.position);
	nrect backGroundRect({0, 0}, geometry.size + nvec2{8, 8});
	backGroundRect.setCenter(posRect.center());
	backGroundShape.setSize(backGroundRect.size);
	backGroundShape.setPosition(backGroundRect.position);

	canvas.draw(backGroundShape);
	canvas.draw(healthBarShape);
	canvas.draw(healthShape);

	nrect textRect = text.getGlobalBounds();
	nvec2 pos = textRect.setCenter(posRect.center()).position;
	pos.y -= 0.5f;
	text.setPosition(pos);
	canvas.draw(text);
}
