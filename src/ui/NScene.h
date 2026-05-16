#pragma once
#ifndef NSCENE_H
#define NSCENE_H
#include "src/global/Register.h"
#include "widget/NWidget.h"
#include <list>

class NScene {
	friend class NWindow;
protected:
	//m_widget updates first, render last (handled by NWindow)
	NWidget* m_widget;

public:
	NScene() :m_widget(nullptr) {}
	virtual ~NScene() {
		if (m_widget) {
			delete m_widget;
		}
	}
	virtual void draw(Renderer& renderer) = 0;
	virtual void update(float deltaTime) = 0;
	virtual void handleEvent(const sf::Event& event) {}

	void setWidget(NWidget* widget) {
		if (!widget) throw std::runtime_error("Widget is nullptr");
		m_widget = widget;
	}

	virtual void enter() {}
	virtual void exit() {}

	virtual std::string_view getName()const = 0;
};

#endif


#define N_REG_SCENE(Class)\
public:\
std::string_view getName()const override { return #Class; }