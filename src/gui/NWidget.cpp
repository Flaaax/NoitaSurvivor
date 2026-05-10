#include"NWidget.h"
#include"NWindow.h"
#include"./Renderer.h"


NWidget::NWidget(bool enableUpdate) :NObject(nullptr), m_focus(nullptr) {
	m_enable_update = enableUpdate;
	m_geometry = NWindow::scale.windowSizeRect;
}

bool NWidget::handleEvent(const sf::Event& event) {
	for (auto* obj : m_objects) {
		if (obj->isVisible() && obj->handleEvent(event)) {
			return true;
		}
	}
	return false;
}

void NWidget::update(float deltaTime) {
	if (!m_enable_update)return;
	for (auto* obj : m_objects) {
		if (obj->m_enable_update && obj->isVisible()) {
			obj->update(deltaTime);
		}
	}
}

void NWidget::draw(Renderer& renderer) const {
	/*sf::RectangleShape shape;
	shape.setSize(m_geometry.size());
	shape.setPosition(renderPos);
	shape.setFillColor(sf::Color(200, 200, 200));
	renderer.drawGui(shape);*/

	renderer.guiContentState.transform.translate(this->m_geometry.position);

	for (auto* obj : m_objects) {
		if (obj->isVisible()) {
			obj->draw(renderer);
		}
	}

	renderer.guiContentState.transform.translate(-this->m_geometry.position);
}
