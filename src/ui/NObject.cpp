#include "NObject.h"

#include "global/Global.h"
#include "widget/NRootWidget.h"
#include "widget/NWidget.h"

namespace flx::ui {
	vec2 NObject::getGlobalPosition() const {
		return toGlobalPosition(frame.position);
	}

	rect NObject::getGlobalBounds() const {
		return rect{getGlobalPosition(), getSize()};
	}

	vec2 NObject::toGlobalPosition(vec2 localPosition) const {
		vec2 ret = localPosition;
		if (const NWidget* parent = getParent()) {
			ret += parent->getGlobalPosition();
		}
		return ret;
	}

	rect NObject::toGlobalBounds(rect parentLocalBounds) const {
		return rect{toGlobalPosition(parentLocalBounds.position), parentLocalBounds.size};
	}

	vec2 NObject::getParentLocalPosition(vec2 globalPosition) const {
		vec2 ret = globalPosition;
		if (const NWidget* parent = getParent()) {
			ret -= parent->getGlobalPosition();
		}
		return ret;
	}

	rect NObject::getLocalBounds() const {
		return {{}, getSize()};
	}

	NWidget* NObject::asWidget() {
		if (!isWidget()) {
			getLogger().error_and_throw("Object is not a widget");
		}
		return static_cast<NWidget*>(this);
	}

	const NViewport& NObject::getGlobalViewport() const {
		if (const auto root = getRoot()) {
			return root->getViewport();
		}

		getLogger().error_and_throw("Object does not have root");
	}

	flx::Logger& NObject::getLogger() {
		return Global::getLogger();
	}

	const NRootWidget* NObject::getRoot() const {
		if (isWidget()) {
			const auto widget = static_cast<const NWidget*>(this);
			if (widget->isRoot) {
				return static_cast<const NRootWidget*>(widget);
			}
		}

		if (const auto parent = getParent()) {
			return parent->getRoot();
		}

		return {};
	}
}