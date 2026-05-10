#pragma once

#include"./NObject.h"
#include"src/utils/NString.h"
#include<functional>


class NButton :public NObject {
public:
	mutable sf::Text text;
	std::function<void()> onClick;

	bool isButtonVisible = true;

	enum :int {
		Hovered,
		Normal,
		Pressed
	}state = Normal;

	NButton() :NButton(0, 0, 50, 50) {}
	NButton(const nrect& geometry) {
		m_geometry = geometry;
		text.setFillColor({ 0,0,0 });
	}
	NButton(float x, float y, float w, float h) {
		m_geometry = { x,y,w,h };
		text.setFillColor({ 0,0,0 });
	}

	bool handleEvent(const sf::Event& event)override;
	void draw(Renderer& renderer)const override;

	sf::Text& getText();
	void setText(const NString& text, unsigned int characterSize = 30);

	void setOnClick(const std::function<void()>& func) {
		onClick = func;
	}
};