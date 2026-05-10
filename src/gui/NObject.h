#pragma once
#ifndef NOBJECT_H
#define NOBJECT_H

#pragma warning(push)
#pragma warning(disable:5260)		//something in b2_math.h
//#include"Renderer.h"
#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif
#include"src/utils/Vec2.h"
#include"src/utils/Container.h"
#include<SFML/Graphics.hpp>
#include"Renderer.h"
#include"src/utils/TypeName.h"

class Renderer;
class NWidget;

namespace sf {
class Event;
}


class NObject : public Util::IntrusiveNode {
	friend class NWidget;

private:
	NWidget* m_parent = nullptr;
	bool m_visible;
	bool m_enable_update;

protected:
	nrect m_geometry;

public:
	NObject() : m_parent(nullptr), m_visible(true), m_enable_update(false) {
	}

	NObject(NWidget* parent) : m_parent(parent), m_visible(true), m_enable_update(false) {
	}

	virtual ~NObject() {
	}

	virtual bool handleEvent(const sf::Event& event) { return false; }

	virtual void update(float deltaTime) {
	}

	virtual void draw(Renderer& renderer) const = 0;

	//Generally, these values should not be changed inside Objects whose geometries are determined by user
	void setPosition(const nvec2& pos) { m_geometry.position = pos; }
	void setSize(const nvec2& pos) { m_geometry.size = pos; }
	void setGeometry(const nrect geometry) { m_geometry = geometry; }
	const nvec2& getPosition() const { return m_geometry.position; }
	const nrect& getGeometry() const { return m_geometry; }
	const nvec2& getSize() const { return m_geometry.size; }

	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() const { return m_visible; }
	void setUpdate(bool enabled) { m_enable_update = enabled; }
	bool getEnableUpdate() const { return m_enable_update; }

	NWidget* getParent() const { return m_parent; }

	std::string_view ID{};
};

#pragma warning(pop)
#endif //ifndef NOBJECT_H