// ReSharper disable CppDFANullDereference
#include "ValueBar.h"
#include "../../utils/Text/NString.h"
#include "src/global/LocManager.h"
#include "src/ui/elements/NRichText.h"
#include "src/ui/global/NGlobal.h"
#include "src/ui/render/NCanvas.h"
#include "src/utils/Text/Format.h"

ValueBar::ValueBar(nvec2 topRight, nvec2 size, int initialMaxHealth, float lengthPerHealth, int mode)
	: m_initialHealth(initialMaxHealth),
	  m_initialLength(size.x),
	  lengthPerHealth(lengthPerHealth),
	  m_right(topRight.x),
	  mode(mode), text(NGlobal::getDefaultFont(), "", static_cast<unsigned int>(size.y * 0.9)) {
	frame.size = size;
	frame.setRightTop(topRight);

	auto style = text.getDefaultStyle();
	style.color = {240, 240, 240};
	style.effects.add(Util::TextEffect::Bold);
	text.setDefaultStyle(style);
	text.setOutlineColor({0, 0, 0});
	text.setOutlineThickness(1.5f);

	backGroundShape.setFillColor(sf::Color::Black);
	backGroundShape.setRadius(6);
	healthBarShape.setFillColor({100, 100, 100});
	if (mode == 0) {
		healthShape.setFillColor({210, 0, 0});
	} else if (mode == 1) {
		healthShape.setFillColor({0, 210, 0});
	}

	setHealth(initialMaxHealth);
	setMaxHealth(initialMaxHealth);
}

void ValueBar::updateText() {
	if (mode == 1) {
		if (format.empty()) {
			const auto levelString = LocManager::inst().debugGetString("ui", "level");
			format = levelString + "{}";
		}
		text.setString(Util::format(format, level));
	} else if (mode == 0) {
		text.setString(Util::format("{} / {}", m_health, m_maxHealth));
	}
}

void ValueBar::draw(const NCanvas& canvas) const {
	const nvec2 healthShapeSize = {(m_health / static_cast<float>(m_maxHealth)) * frame.size.x, frame.size.y};
	healthShape.setSize(healthShapeSize);
	healthShape.setPosition({frame.size.x - healthShapeSize.x, 0});

	const nrect posRect = getLocalBounds();

	healthBarShape.setSize(getSize());

	nrect backGroundRect({0, 0}, getSize() + nvec2{8, 8});
	backGroundRect.setCenter(posRect.center());
	backGroundShape.setSize(backGroundRect.size);
	backGroundShape.setPosition(backGroundRect.position);

	canvas.draw(backGroundShape);
	canvas.draw(healthBarShape);
	canvas.draw(healthShape);

	const nvec2 textSize = text.getLayoutSize();
	nvec2 pos = nrect::fromCenter(posRect.center(), textSize).position;
	pos.y -= 0.5f;
	text.setPosition(pos);
	canvas.draw(text);
}
