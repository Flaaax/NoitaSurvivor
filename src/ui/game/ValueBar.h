#pragma once

#include "../NObject.h"
#include "../shapes/NRoundRectShape.h"
#include <SFML/Graphics.hpp>


//Used for both health bar and exp bar
class ValueBar :public NObject {
public:
	inline static constexpr int HEALTH = 0;
	inline static constexpr int EXP = 1;
private:
	const int m_initialHealth;
	const float m_initialLength;
	int m_health;
	int m_maxHealth;
	float lengthPerHealth;
	const float m_right;

	int mode = 0;
	int level = 0;

	mutable NRoundRectShape backGroundShape;
	mutable sf::RectangleShape healthBarShape;
	mutable sf::RectangleShape healthShape;
	mutable sf::Text text;

public:
	ValueBar(const nvec2& topRight, const nvec2& size, int initialMaxHealth, float lengthPerHealth = 2.f, int mode = 0);
	void setHealth(int health) {
		m_health = health;
		updateText();
	}

	void setMaxHealth(int maxHealth) {
		m_maxHealth = maxHealth;
		geometry.w = lengthPerHealth * (m_maxHealth - m_initialHealth) + m_initialLength;
		geometry.x = m_right - geometry.w;
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

	void draw(const NCanvas& canvas) const override;
};