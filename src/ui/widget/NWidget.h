#pragma once
#include "../NObject.h"
#include "src/ui/context/NDragState.h"
#include "src/utils/Assert.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

#include <deque>

namespace sf {
	class Event;
}

// Use NObject::setParent to add an object
class NWidget : public NObject {
private:
	friend class NObject;
	friend class NRootWidget;
	Util::Vector<n_unique<NObject>> objects;

	void bind(NObject* obj) {
		assertNotNull(obj);
		if (obj == this) {
			Logger::error_and_throw("Cannot bind a widget to itself!");
		}
		if (obj->parent) {
			obj->parent->remove(obj);
		}
		obj->parent = this;
	}

public:
	// geomoetry will be ignored if this is the root widget
	explicit NWidget(nrect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true) : NObject(nullptr) {
		updateEnabled = updateEnabled_;
		this->geometry = geometry;
		isWidget_ = true;
	}

	NObject* add(n_unique<NObject> obj) {
		return addToTop(std::move(obj));
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

	std::optional<NEventResult> handleEvent(const NUIEvent& event) override;

	void update(float deltaTime) override;
	void draw(const NCanvas& canvas) const override;

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

	void onDropQuery(const NDropQuery& query, NDropCollector& collector) override;
};