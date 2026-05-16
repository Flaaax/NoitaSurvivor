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
#include "Renderer.h"
#include "context/NEvent.h"
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

private:
	NWidget* parent{};
	bool isWidget_{};
	bool isDragged{};

protected:
	nrect m_geometry;

public:
	bool enableDragging{};
	bool updateEnabled{};
	bool isVisible{};

	NObject() : isVisible(true) {
	}

	explicit NObject(NWidget* parent) : parent(parent), isVisible(true) {
	}

	virtual ~NObject() {
	}

	bool isWidget() const {
		return this->isWidget_;
	}

	virtual std::optional<NEventResult> handleEvent(const NEvent& event) {
		return std::nullopt;
	}

	virtual void update(float deltaTime) {
	}

	virtual void draw(Renderer& renderer) const = 0;

	// Generally, these values should not be changed inside Objects whose geometries are determined by user
	void setPosition(const nvec2& pos) {
		m_geometry.position = pos;
	}

	void setSize(const nvec2& pos) {
		m_geometry.size = pos;
	}

	void setGeometry(const nrect geometry) {
		m_geometry = geometry;
	}

	nvec2 getPosition() const {
		return m_geometry.position;
	}

	nrect getGeometry() const {
		return m_geometry;
	}

	nvec2 getSize() const {
		return m_geometry.size;
	}

	NWidget* getParent() const {
		return parent;
	}

	nvec2 getGlobalPosition()const;

	NWidget* asWidget();

	std::string_view ID{};
};

#pragma warning(pop)
#endif // ifndef NOBJECT_H