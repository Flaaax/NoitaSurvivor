#pragma once
#include "../NObject.h"
#include "src/ui/context/NDragState.h"
#include "src/utils/Container/Vector.h"

#include <deque>

namespace sf {
	class Event;
}

// Use NObject::setParent to add an object
class NWidget : public NObject {
private:
	friend class NObject;
	Util::Vector<n_unique<NObject>> objects;
	NWidgetCtx widgetCtx;

	void bind(NObject* obj) {
		assertNotNull(obj);
		if (obj->parent) {
			obj->parent->remove(obj);
		}
		obj->parent = this;
	}

public:
	// geomoetry will be ignored if this is the root widget
	explicit NWidget(nrect geometry, bool enableUpdate = true) : NObject(nullptr) {
		updateEnabled = enableUpdate;
		m_geometry = geometry;
		isWidget_ = true;
	}

	NObject* addToTop(n_unique<NObject> obj) {
		assertWithMsg(!has(obj.get()), "Should not re-add object, use moveTo* instead!");
		bind(obj.get());
		return objects.emplace_back(std::move(obj)).get();
	}

	NObject* addToBottom(n_unique<NObject> obj) {
		assertWithMsg(!has(obj.get()), "Should not re-add object, use moveTo* instead!");
		bind(obj.get());
		return objects.emplace_front(std::move(obj)).get();
	}

	auto& getObjects() const {
		return objects;
	}

	std::optional<NEventResult> handleEvent(const NEvent& event) override;

	// Handle event as the root widget
	bool handleEvent(const NEventCtx& ctx);

	void update(float deltaTime) override;
	void draw(Renderer& renderer) const override;

	bool has(const NObject* obj) const {
		return obj->getParent() == this;
	}

	n_unique<NObject> remove(const NObject* target);

	void moveToTop(const NObject* obj) {
		assertWithMsg(has(obj), "Does not own current object");
		addToTop(remove(obj));
	}

	void moveToBottom(const NObject* obj) {
		assertWithMsg(has(obj), "Does not own current object");
		addToBottom(remove(obj));
	}

	virtual void onDropAccepted(NDragState state) {
	}
};