#include "NObject.h"

#include "global/NGlobal.h"
#include "widget/NRootWidget.h"
#include "widget/NWidget.h"

nvec2 NObject::getGlobalPosition() const {
	return toGlobalPosition(frame.position);
}

nrect NObject::getGlobalBounds() const {
	return nrect{getGlobalPosition(), getSize()};
}

nvec2 NObject::toGlobalPosition(nvec2 localPosition) const {
	nvec2 ret = localPosition;
	if (const NWidget* parent = getParent()) {
		ret += parent->getGlobalPosition();
	}
	return ret;
}

nrect NObject::toGlobalBounds(nrect parentLocalBounds) const {
	return nrect{toGlobalPosition(parentLocalBounds.position), parentLocalBounds.size};
}

nvec2 NObject::getParentLocalPosition(nvec2 globalPosition) const {
	nvec2 ret = globalPosition;
	if (const NWidget* parent = getParent()) {
		ret -= parent->getGlobalPosition();
	}
	return ret;
}

nrect NObject::getLocalBounds() const {
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
	return NGlobal::getLogger();
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