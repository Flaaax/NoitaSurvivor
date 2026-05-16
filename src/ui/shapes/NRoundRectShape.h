#pragma once
#include<SFML/Graphics/Shape.hpp>
#include<vector>
#include"src/utils/Vec2.h"

class NRoundRectShape : public sf::Shape {
public:
	explicit NRoundRectShape() : m_radius(0) {}
	explicit NRoundRectShape(fvec2 size, float radius)
		: m_size(size), m_radius(radius) {
		updatePoints();
	}

	virtual size_t getPointCount() const override {
		return m_points.size();
	}

	virtual fvec2 getPoint(size_t index) const override {
		return m_points[index];
	}

	void setSize(const fvec2& size) {
		m_size = size;
		updatePoints();
	}

	void setRadius(float radius) {
		m_radius = radius;
		updatePoints();
	}

	void set(const fvec2& size, float radius) {
		m_size = size;
		m_radius = radius;
		updatePoints();
	}

private:
	void updatePoints();

	fvec2 m_size;
	float m_radius;
	std::vector<fvec2> m_points;
	size_t m_segments = 10; // Rounded-corner segment count.
};
