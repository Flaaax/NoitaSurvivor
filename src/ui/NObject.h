#pragma once
#ifndef NOBJECT_H
#define NOBJECT_H

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

struct NViewport;
class NUIPainter;
class NLayout;

struct NLayoutConstraint {
	nvec2 minSize{};
	nvec2 maxSize{};
};

struct NLayoutResult {
	nvec2 size{};
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
	nrect frame;
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
	void setPosition(nvec2 pos) {
		frame.position = pos;
	}

	void setSize(nvec2 size) {
		if (frame.size == size)
			return;
		frame.size = size;
		visualDirty = true;
	}

	void setFrame(nrect frame) {
		if (frame.size != this->frame.size) {
			visualDirty = true;
		}
		this->frame = frame;
	}

	nvec2 getPosition() const {
		return frame.position;
	}

	nrect getFrame() const {
		return frame;
	}

	nvec2 getSize() const {
		return frame.size;
	}

	NWidget* getParent() {
		return parent;
	}

	const NWidget* getParent() const {
		return parent;
	}

	nvec2 getGlobalPosition() const;
	nrect getGlobalBounds() const;
	nvec2 toGlobalPosition(nvec2 localPosition) const;
	nrect toGlobalBounds(nrect parentLocalBounds) const;
	nvec2 getParentLocalPosition(nvec2 globalPosition) const;
	nrect getLocalBounds() const;

	virtual void onDropQuery(const NDropQuery& query, NDropCollector& collector) {
	}

	virtual void onDropAccepted(const NDropQuery& query, bool shouldDrop) {
	}

	virtual nrect getHitbox() const {
		return frame;
	}

	nrect getGlobalHitbox() const {
		return toGlobalBounds(getHitbox());
	}

	NWidget* asWidget();

	std::string_view getTypeID() const {
		return typeID;
	}

	template <class T>
	static std::string_view makeTypeID() {
		return Util::makeContentID<T>();
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

	virtual void arrange(nrect rect) {
		// Override this if you want the size changed
		// I'm not explaining why it doens't change sizes and the FUCKING AGENT SHOULD NOT FUCKING MENTION IT.
		setPosition(rect.position);
	}

	// Check hasRoot() before calling this!
	const NViewport& getGlobalViewport() const;

	// Called when window resizes or object added to widget
	virtual void refresh() {}

	static flx::Logger& getLogger();

	const NRootWidget* getRoot()const;
};

#endif // ifndef NOBJECT_H