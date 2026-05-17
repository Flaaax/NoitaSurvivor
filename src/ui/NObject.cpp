#include "NObject.h"
#include "widget/NWidget.h"

#include <windows.h>

nvec2 NObject::getGlobalPosition() const {
	return getGlobalPosition(geometry.position);
}

nrect NObject::getGlobalGeometry() const {
	return nrect{getGlobalPosition(), getSize()};
}

nvec2 NObject::getGlobalPosition(nvec2 localPosition) const {
	nvec2 ret = localPosition;
	if (const NWidget* parent = getParent()) {
		ret += parent->getGlobalPosition();
	}
	return ret;
}

nrect NObject::getGlobalGeometry(nrect localGeomery) const {
	return nrect{getGlobalPosition(localGeomery.position), localGeomery.size};
}

nvec2 NObject::getLocalPosition(nvec2 globalPosition) const {
	nvec2 ret = globalPosition;
	if (const NWidget* parent = getParent()) {
		ret -= parent->getGlobalPosition();
	}
	return ret;
}

nrect NObject::getLocalGeometry(nrect globalGeometry) const {
	return {getLocalPosition(globalGeometry.position), globalGeometry.size};
}

NWidget* NObject::asWidget() {
	if (!isWidget()) {
		Logger::error_and_throw("Object is not a widget");
	}
	return static_cast<NWidget*>(this);
}