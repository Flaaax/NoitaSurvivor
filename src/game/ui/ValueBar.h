#pragma once

#include "src/ui/Object.h"
#include "src/ui/shapes/RichTextShape.h"
#include "src/ui/shapes/RoundRectShape.h"

#include <SFML/Graphics.hpp>

namespace flx::ui {
	class RichText;

	// Used for both health bar and exp bar
	class ValueBar : public Object {
	public:
		static constexpr int HEALTH = 0;
		static constexpr int EXP = 1;

	private:
		const int m_initialHealth;
		const float m_initialLength;
		int m_health;
		int m_maxHealth;
		float lengthPerHealth;
		const float m_right;

		int mode = 0;
		int level = 0;

		mutable RoundRectShape backGroundShape;
		mutable sf::RectangleShape healthBarShape;
		mutable sf::RectangleShape healthShape;
		mutable RichTextShape text;

		inline static std::string format{};

	public:
		ValueBar(vec2 topRight, vec2 size, int initialMaxHealth, float lengthPerHealth = 2.f, int mode = 0);

		void setHealth(int health) {
			m_health = health;
			updateText();
		}

		void setMaxHealth(int maxHealth) {
			m_maxHealth = maxHealth;
			frame.size.x = lengthPerHealth * (m_maxHealth - m_initialHealth) + m_initialLength;
			frame.position.x = m_right - frame.size.x;
			updateText();
		}

		void setValue(int val, int maxVal, int lvl = 0) {
			m_health = val;
			m_maxHealth = maxVal;
			level = lvl;
			updateText();
		}

		void setLevel(int lvl) {
			level = lvl;
			updateText();
		}

		void updateText();

		void draw(const UIPainter& canvas) const override;
	};
} // namespace flx::ui