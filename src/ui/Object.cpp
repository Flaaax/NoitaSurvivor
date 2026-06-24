#include "Object.h"

#include "global/Global.h"
#include "widget/NRootWidget.h"
#include "widget/NWidget.h"

namespace flx::ui {
	vec2 Object::getGlobalPosition() const {
		return toGlobalPosition(frame.position);
	}

	rect Object::getGlobalBounds() const {
		return rect{getGlobalPosition(), getSize()};
	}

	vec2 Object::toGlobalPosition(vec2 localPosition) const {
		vec2 ret = localPosition;
		if (const Widget* parent = getParent()) {
			ret += parent->getGlobalPosition();
		}
		return ret;
	}

	rect Object::toGlobalBounds(rect parentLocalBounds) const {
		return rect{toGlobalPosition(parentLocalBounds.position), parentLocalBounds.size};
	}

	vec2 Object::getParentLocalPosition(vec2 globalPosition) const {
		vec2 ret = globalPosition;
		if (const Widget* parent = getParent()) {
			ret -= parent->getGlobalPosition();
		}
		return ret;
	}

	rect Object::getLocalBounds() const {
		return {{}, getSize()};
	}

	Widget* Object::asWidget() {
		if (!isWidget()) {
			getLogger().error_and_throw("Object is not a widget");
		}
		return static_cast<Widget*>(this);
	}

	const Viewport& Object::getGlobalViewport() const {
		if (const auto root = getRoot()) {
			return root->getViewport();
		}

		getLogger().error_and_throw("Object does not have root");
	}

	flx::Logger& Object::getLogger() {
		return Global::getLogger();
	}

	const RootWidget* Object::getRoot() const {
		if (isWidget()) {
			const auto widget = static_cast<const Widget*>(this);
			if (widget->isRoot) {
				return static_cast<const RootWidget*>(widget);
			}
		}

		if (const auto parent = getParent()) {
			return parent->getRoot();
		}

		return {};
	}
}