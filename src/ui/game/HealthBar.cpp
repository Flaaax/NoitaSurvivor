#include "HealthBar.h"
#include "../Renderer.h"
#include "src/global/AssetManager.h"
#include "src/utils/NString.h"

HealthBar::HealthBar(const nvec2& topRight, const nvec2& size, int initialMaxHealth, float lengthPerHealth, int mode)
	: m_initialHealth(initialMaxHealth),
	  m_initialLength(size.x),
	  lengthPerHealth(lengthPerHealth),
	  m_right(topRight.x),
	  mode(mode), text(AssetMgr::getDefaultFont()) {
	setHealth(initialMaxHealth);
	setMaxHealth(initialMaxHealth);
	m_geometry.size = size;
	m_geometry.setTopRight(topRight);

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

void HealthBar::updateText() {
	if (mode == 1) {
		text.setString(NString("等级{}", level));
	} else if (mode == 0) {
		text.setString(NString("{} / {}", m_health, m_maxHealth));
	}
}

void HealthBar::draw(Renderer& renderer) const {
	auto& renderPos = m_geometry.position;
	const fvec2 healthShapeSize = {(m_health / static_cast<float>(m_maxHealth)) * m_geometry.w, m_geometry.h};
	healthShape.setSize(healthShapeSize);
	healthShape.setPosition({renderPos.x + m_geometry.w - healthShapeSize.x, renderPos.y});

	nrect posRect = {renderPos, m_geometry.size};

	healthBarShape.setSize(m_geometry.size);

	healthBarShape.setPosition(posRect.position);
	nrect backGroundRect({0, 0}, m_geometry.size + nvec2{8, 8});
	backGroundRect.setCenter(posRect.center());
	backGroundShape.setSize(backGroundRect.size);
	backGroundShape.setPosition(backGroundRect.position);

	renderer.drawGui(backGroundShape);
	renderer.drawGui(healthBarShape);
	renderer.drawGui(healthShape);

	nrect textRect = text.getGlobalBounds();
	nvec2 pos = textRect.setCenter(posRect.center()).position;
	pos.y -= 0.5f;
	text.setPosition(pos);
	renderer.drawGui(text);
}
