#include "NObject.h"
#include "widget/NWidget.h"

#include <windows.h>

nvec2 NObject::getGlobalPosition() const {
	nvec2 ret = getPosition();
	if (const NWidget* parent = getParent()) {
		ret += parent->getGlobalPosition();
	}
	return ret;
}
NWidget* NObject::asWidget() {
	if (!isWidget()) {
		return {};
	}
	return static_cast<NWidget*>(this);
}