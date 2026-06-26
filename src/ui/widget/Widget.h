#pragma once
#include "../Object.h"
#include "src/ui/context/States.h"
#include "src/utils/Assert.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

namespace sf {
	class Event;
}

namespace flx::ui {
	// Use NObject::setParent to add an object
	class Widget : public Object {
	private:
		friend class Object;
		friend class RootWidget;

		bool isRoot{};
		Vector<SUnique<Object>> objects;

		void bind(Object* obj);
		static void drawWithChildCanvas(const UIPainter& canvas, const Object& obj);

	public:
		explicit Widget(rect geometry = {0.f, 0.f, 100.f, 100.f}, bool updateEnabled_ = true) {
			updateEnabled = updateEnabled_;
			this->frame = geometry;
			isWidget_ = true;
		}

		Object* add(SUnique<Object> obj) {
			return addToTop(std::move(obj));
		}

		// template <std::derived_from<NObject> T>
		// T* add(n_unique<T> obj) {
		// 	return static_cast<T*>(this->add(std::move(obj)));
		// }

		Object* addToTop(SUnique<Object> obj) {
			assertWithMsg(!has(obj), "Should not re-add object, use moveTo* instead!");
			bind(obj.get());
			obj->refresh();
			return objects.emplace_back(std::move(obj)).get();
		}

		Object* addToBottom(SUnique<Object> obj) {
			assertWithMsg(!has(obj), "Should not re-add object, use moveTo* instead!");
			bind(obj.get());
			obj->refresh();
			return objects.emplace_front(std::move(obj)).get();
		}

		auto getObjects() const {
			return objects | std::views::transform([](const SUnique<Object>& obj) { return obj.ref(); });
		}

		std::optional<EventResult> handleEvent(const UIEvent& event) override;

		void update(float deltaTime) override;
		void draw(const UIPainter& canvas) const override;

		bool has(const CRef<Object>& obj) const {
			return obj->getParent() == this;
		}

		SUnique<Object> remove(const Ref<Object>& target);
		void clear();

		void moveToTop(const Ref<Object>& obj) {
			assertWithMsg(has(obj), "Does not own current object");
			addToTop(remove(obj));
		}

		void moveToBottom(const Ref<Object>& obj) {
			assertWithMsg(has(obj), "Does not own current object");
			addToBottom(remove(obj));
		}

		void onDropQuery(const DropQuery& query, DropCollector& collector) override;

		void refresh() override;
	};
} // namespace flx::ui