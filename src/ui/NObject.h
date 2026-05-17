#pragma once
#ifndef NOBJECT_H
#define NOBJECT_H

#pragma warning(push)
#pragma warning(disable : 5260) // something in b2_math.h
// #include"Renderer.h"
#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif
#include "../utils/Container/Container.h"
#include "context/NUIEvent.h"
#include "render/NCanvas.h"
#include "src/utils/TypeName.h"
#include "src/utils/Vec2.h"
#include <SFML/Graphics.hpp>

class Renderer;
class NWidget;

namespace sf {
	class Event;
}

class NObject {
	friend class NWidget;
	friend class NRootWidget;

private:
	NWidget* parent{};
	bool isWidget_{};
	bool isDragged_{};

protected:
	nrect geometry;
	std::string_view typeID{};

public:
	bool enableDragging{};
	bool updateEnabled{};
	bool isVisible = true;

	explicit NObject(NWidget* parent = {})
		: parent(parent) {
	}

	virtual ~NObject() {
	}

	bool isWidget() const {
		return this->isWidget_;
	}

	bool isDragged() const {
		return this->isDragged_;
	}

	virtual std::optional<NEventResult> handleEvent(const NUIEvent& event) {
		return std::nullopt;
	}

	virtual void update(float deltaTime) {
	}

	virtual void draw(const NCanvas& canvas) const = 0;

	// Generally, these values should not be changed inside Objects whose geometries are determined by user
	void setPosition(const nvec2& pos) {
		geometry.position = pos;
	}

	void setSize(const nvec2& pos) {
		geometry.size = pos;
	}

	void setGeometry(const nrect geometry) {
		this->geometry = geometry;
	}

	nvec2 getPosition() const {
		return geometry.position;
	}

	nrect getGeometry() const {
		return geometry;
	}

	nvec2 getSize() const {
		return geometry.size;
	}

	NWidget* getParent() const {
		return parent;
	}

	nvec2 getGlobalPosition() const;
	nrect getGlobalGeometry() const;
	nvec2 getGlobalPosition(nvec2 localPosition) const;
	nrect getGlobalGeometry(nrect localGeomery) const;
	nvec2 getLocalPosition(nvec2 globalPosition) const;
	nrect getLocalGeometry(nrect globalGeometry) const;

	virtual void onDropQuery(const NDropQuery& query, NDropCollector& collector) {
	}

	virtual void onDropAccepted(const NDropQuery& query, bool shouldDrop) {
	}

	virtual nrect getHitbox() const {
		return geometry;
	}

	nrect getGlobalHitbox() const {
		return getGlobalGeometry(getHitbox());
	}

	NWidget* asWidget();

	std::string_view getTypeID() const {
		return typeID;
	}

	template <class T>
	static constexpr std::string_view makeTypeID() {
		return Util::typeName<T>();
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
};

#pragma warning(pop)
#endif // ifndef NOBJECT_H