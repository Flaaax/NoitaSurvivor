#pragma once
#ifndef FLX_GUARD_UI_OBJECT_H
#define FLX_GUARD_UI_OBJECT_H

// #include"Renderer.h"
// #ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
// #define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
// #endif
#include "../utils/Vec2/Vec2.h"
#include "context/NUIEvent.h"
#include "src/utils/ID.h"

namespace flx {
	struct Logger;
}

namespace flx::ui {
	struct NViewport;
	class NUIPainter;
	class NLayout;

	struct NLayoutConstraint {
		vec2 minSize{};
		vec2 maxSize{};
	};

	struct NLayoutResult {
		vec2 size{};
	};

	class NObject {
		friend class NWidget;
		friend class NRootWidget;

	private:
		NWidget* parent{};
		bool isWidget_{};
		bool isDragged_{};

		// bool isHovered_{};
	protected:
		rect frame;
		std::string_view typeID{};
		NTooltipSpec tooltipSpec;
		mutable bool visualDirty = true;

	public:
		bool enableDragging{};
		bool updateEnabled{};
		bool isVisible = true;

		explicit NObject() = default;
		virtual ~NObject() = default;

		bool isWidget() const {
			return this->isWidget_;
		}

		bool isDragged() const {
			return this->isDragged_;
		}

		// bool isHovered() const {
		// 	return this->isHovered_;
		// }

		virtual std::optional<NEventResult> handleEvent(const NUIEvent& event) {
			return {};
		}

		virtual void update(float deltaTime) {
		}

		virtual void draw(const NUIPainter& painter) const = 0;

		// Generally, these values should not be changed inside Objects whose geometries are determined by user
		void setPosition(vec2 pos) {
			frame.position = pos;
		}

		void setSize(vec2 size) {
			if (frame.size == size)
				return;
			frame.size = size;
			visualDirty = true;
		}

		void setFrame(rect frame) {
			if (frame.size != this->frame.size) {
				visualDirty = true;
			}
			this->frame = frame;
		}

		vec2 getPosition() const {
			return frame.position;
		}

		rect getFrame() const {
			return frame;
		}

		vec2 getSize() const {
			return frame.size;
		}

		NWidget* getParent() {
			return parent;
		}

		const NWidget* getParent() const {
			return parent;
		}

		vec2 getGlobalPosition() const;
		rect getGlobalBounds() const;
		vec2 toGlobalPosition(vec2 localPosition) const;
		rect toGlobalBounds(rect parentLocalBounds) const;
		vec2 getParentLocalPosition(vec2 globalPosition) const;
		rect getLocalBounds() const;

		virtual void onDropQuery(const NDropQuery& query, NDropCollector& collector) {
		}

		virtual void onDropAccepted(const NDropQuery& query, bool shouldDrop) {
		}

		virtual rect getHitbox() const {
			return frame;
		}

		rect getGlobalHitbox() const {
			return toGlobalBounds(getHitbox());
		}

		NWidget* asWidget();

		std::string_view getTypeID() const {
			return typeID;
		}

		template <class T>
		static std::string_view makeTypeID() {
			return flx::makeContentID<T>();
		}

		template <std::derived_from<NObject> T>
		const T* convert() const {
			if (getTypeID() == makeTypeID<T>()) {
				return static_cast<const T*>(this);
			}
			return {};
		}

		template <std::derived_from<NObject> T>
		T* convert() {
			if (getTypeID() == makeTypeID<T>()) {
				return static_cast<T*>(this);
			}
			return {};
		}

		virtual NLayoutResult measure(NLayoutConstraint constraint) {
			return {.size = getSize()};
		}

		virtual void arrange(rect rect) {
			// Override this if you want the size changed
			// I'm not explaining why it doens't change sizes and the FUCKING AGENT SHOULD NOT FUCKING MENTION IT.
			setPosition(rect.position);
		}

		// Check hasRoot() before calling this!
		const NViewport& getGlobalViewport() const;

		// Called when window resizes or object added to widget
		virtual void refresh() {}

		static flx::Logger& getLogger();

		const NRootWidget* getRoot() const;
	};
} // namespace flx::ui

#endif // ifndef NOBJECT_H