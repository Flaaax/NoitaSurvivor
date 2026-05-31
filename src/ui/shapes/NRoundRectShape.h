#pragma once
#include "../../utils/Vec2/Vec2.h"
#include <SFML/Graphics/Shape.hpp>
#include <vector>

class NRoundRectShape : public sf::Shape {
private:
	mutable std::vector<nvec2> m_points;
	nvec2 m_size;
	float m_radius;
	size_t m_segments = 10; // Rounded-corner segment count.

	void updatePoints();

public:
	explicit NRoundRectShape() : m_radius(0) {}

	explicit NRoundRectShape(nvec2 size, float radius)
		: m_size(size), m_radius(radius) {
		updatePoints();
	}

	size_t getPointCount() const override {
		return m_points.size();
	}

	sf::Vector2f getPoint(size_t index) const override {
		return m_points[index];
	}

	void setSize(nvec2 size) {
		m_size = size;
		updatePoints();
	}

	void setRadius(float radius) {
		m_radius = radius;
		updatePoints();
	}

	void set(nvec2 size, float radius) {
		m_size = size;
		m_radius = radius;
		updatePoints();
	}
};
