// ReSharper disable CppDFANullDereference
#include "ValueBar.h"
#include "../../app/global/LocManager.h"
#include "src/ui/elements/RichText.h"
#include "src/ui/global/Global.h"
#include "src/ui/render/Painter.h"
#include "src/utils/Text/Format.h"

namespace flx::ui {
	ValueBar::ValueBar(vec2 topRight, vec2 size, int initialMaxHealth, float lengthPerHealth, int mode)
		: m_initialHealth(initialMaxHealth),
		  m_initialLength(size.x),
		  lengthPerHealth(lengthPerHealth),
		  m_right(topRight.x),
		  mode(mode), text(Global::getDefaultFont(), "", static_cast<unsigned int>(size.y * 0.9)) {
		frame.size = size;
		frame.setRightTop(topRight);

		auto style = text.getDefaultStyle();
		style.color = {240, 240, 240};
		style.effects.add(text::TextEffect::Bold);
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
				const auto levelString = app::LocManager::debugGetString("ui", "level");
				format = levelString + "{}";
			}
			text.setString(flx::vformat(format, level));
		} else if (mode == 0) {
			text.setString(flx::vformat("{} / {}", m_health, m_maxHealth));
		}
	}

	void ValueBar::draw(const UIPainter& canvas) const {
		const vec2 healthShapeSize = {(m_health / static_cast<float>(m_maxHealth)) * frame.size.x, frame.size.y};
		healthShape.setSize(healthShapeSize);
		healthShape.setPosition({frame.size.x - healthShapeSize.x, 0});

		const rect posRect = getLocalBounds();

		healthBarShape.setSize(getSize());

		rect backGroundRect({0, 0}, getSize() + vec2{8, 8});
		backGroundRect.setCenter(posRect.center());
		backGroundShape.setSize(backGroundRect.size);
		backGroundShape.setPosition(backGroundRect.position);

		canvas.draw(backGroundShape);
		canvas.draw(healthBarShape);
		canvas.draw(healthShape);

		const vec2 textSize = text.getLayoutSize();
		vec2 pos = rect::fromCenter(posRect.center(), textSize).position;
		pos.y -= 0.5f;
		text.setPosition(pos);
		canvas.draw(text);
	}
} // namespace flx::ui
