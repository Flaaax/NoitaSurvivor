#pragma once
#include "../NObject.h"
#include "src/ui/context/States.h"
#include "src/utils/Assert.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

#include <deque>

namespace sf {
	class Event;
}

namespace flx::ui {
	// Use NObject::setParent to add an object
	class NWidget : public NObject {
	private:
		friend class NObject;
		friend class NRootWidget;

		bool isRoot{};
		flx::Vector<n_unique<NObject>> objects;

		void bind(NObject* obj);

		static void drawWithChildCanvas(const NUIPainter& canvas, const NObject& obj);

	public:
		explicit NWidget(rect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true) {
			updateEnabled = updateEnabled_;
			this->frame = geometry;
			isWidget_ = true;
		}

		NObject* add(n_unique<NObject> obj) {
			return addToTop(std::move(obj));
		}

		// template <std::derived_from<NObject> T>
		// T* add(n_unique<T> obj) {
		// 	return static_cast<T*>(this->add(std::move(obj)));
		// }

		NObject* addToTop(n_unique<NObject> obj) {
			assertWithMsg(!has(obj.get()), "Should not re-add object, use moveTo* instead!");
			bind(obj.get());
			obj->refresh();
			return objects.emplace_back(std::move(obj)).get();
		}

		NObject* addToBottom(n_unique<NObject> obj) {
			assertWithMsg(!has(obj.get()), "Should not re-add object, use moveTo* instead!");
			bind(obj.get());
			obj->refresh();
			return objects.emplace_front(std::move(obj)).get();
		}

		auto getObjects() const {
			return objects | std::views::transform([](const n_unique<NObject>& obj) { return obj.get(); });
		}

		std::optional<NEventResult> handleEvent(const NUIEvent& event) override;

		void update(float deltaTime) override;
		void draw(const NUIPainter& canvas) const override;

		bool has(const NObject* obj) const {
			return obj->getParent() == this;
		}

		n_unique<NObject> remove(const NObject* target);
		void clear();

		void moveToTop(const NObject* obj) {
			assertWithMsg(has(obj), "Does not own current object");
			addToTop(remove(obj));
		}

		void moveToBottom(const NObject* obj) {
			assertWithMsg(has(obj), "Does not own current object");
			addToBottom(remove(obj));
		}

		void onDropQuery(const NDropQuery& query, NDropCollector& collector) override;

		void refresh() override;
	};
} // namespace flx::ui