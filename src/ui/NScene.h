#pragma once
#ifndef NSCENE_H
#define NSCENE_H
#include "widget/NRootWidget.h"
#include "widget/NWidget.h"

class NScene {
protected:
	n_unique<NRootWidget> widget{};

public:
	virtual ~NScene() {
	}

	virtual void draw(Renderer& rdr) {
		if (widget) {
			widget->draw(rdr);
		}
	}

	virtual void update(float dt) {
		if (widget) {
			widget->update(dt);
		}
	}

	virtual bool handleEvent(const NEventCtx& event) {
		if (widget) {
			return widget->handleEvent(event);
		}
		return false;
	}

	void createWidget() {
		widget = Util::makeUnique(new NRootWidget());
	}

	virtual void enter() {
	}

	virtual void exit() {
	}

	virtual std::string_view getName() const = 0;
};

#endif