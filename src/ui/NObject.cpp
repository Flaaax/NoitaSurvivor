#include "NObject.h"
#include "widget/NWidget.h"

#include <windows.h>

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
		Logger::error_and_throw("Object is not a widget");
	}
	return static_cast<NWidget*>(this);
}