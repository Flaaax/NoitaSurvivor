#pragma once
#include"./NObject.h"
#include<deque>


namespace sf {
	class Event;
}

//Use NObject::setParent to add an object
class NWidget :public NObject {
private:
	friend class NObject;
	Util::IntrusiveList<NObject, Util::NoDelete> m_objects;
	NObject* m_focus;

	void bind(NObject* obj) {
		assertNotNull(obj);
		if (obj->m_parent) {
			obj->m_parent->remove(obj);
		}
		obj->m_parent = this;
	}
public:
	NWidget(const nrect& geometry, bool enableUpdate = true) :NObject(nullptr), m_focus(nullptr) {
		m_enable_update = enableUpdate;
		m_geometry = geometry;
	}
	NWidget(bool enableUpdate = true);

	~NWidget() {
		while (m_objects.size() > 0) {
			NObject* back = m_objects.back();
			m_objects.pop_back();
			delete back;
		}
	}

	NObject* add(NObject* obj) {
		return addToTop(obj);
	}

	NObject* addToTop(NObject* obj) {
		bind(obj);
		return m_objects.emplace_back(obj);
	}

	NObject* addToBottom(NObject* obj) {
		bind(obj);
		return m_objects.emplace_front(obj);
	}

	NObject* addAfter(NObject* cur, NObject* obj) {
		assertWithMsg(has(cur), "Does not own current object");
		bind(obj);
		return m_objects.emplace_after(cur, obj);
	}

	NObject* addBefore(NObject* cur, NObject* obj) {
		assertWithMsg(has(cur), "Does not own current object");
		bind(obj);
		return m_objects.emplace_before(cur, obj);
	}

	auto& getObjects()const { return m_objects; }
	auto& getObjects() { return m_objects; }

	bool handleEvent(const sf::Event& event) override;
	void update(float deltaTime)override;
	void draw(Renderer& renderer)const override;

	bool has(NObject* obj) const {
		return obj->getParent() == this;
	}

	//This would not destroy the object
	void remove(NObject* obj) {
		if (!obj || !has(obj)) {
			throw std::runtime_error("Does not contain object");
		}
		m_objects.erase(obj);
	}

	//This will destroy the object
	void destroy(NObject* obj) {
		remove(obj);
		delete obj;
	}

	void moveToTop(NObject* obj) {
		assertWithMsg(has(obj), "Does not own current object");
		addToTop(obj);
	}

	void moveToBottom(NObject* obj) {
		assertWithMsg(has(obj), "Does not own current object");
		addToBottom(obj);
	}

	void focus(NObject* obj) {
		if (!has(obj)) {
			throw std::runtime_error("Object does not exist");
		}
		m_focus = obj;
	}
	void unfocus() {
		m_focus = nullptr;
	}
	NObject* getFocus()const {
		return m_focus;
	}
};