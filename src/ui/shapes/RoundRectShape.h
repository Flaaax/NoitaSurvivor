#pragma once
#include "../../utils/Vec2/Vec2.h"
#include <SFML/Graphics/Shape.hpp>
#include <vector>

namespace flx::ui {
	class RoundRectShape : public sf::Shape {
	private:
		mutable std::vector<vec2> m_points;
		vec2 m_size;
		float m_radius;
		size_t m_segments = 10; // Rounded-corner segment count.

		void updatePoints();

	public:
		explicit RoundRectShape() : m_radius(0) {}

		explicit RoundRectShape(vec2 size, float radius)
			: m_size(size), m_radius(radius) {
			updatePoints();
		}

		size_t getPointCount() const override {
			return m_points.size();
		}

		sf::Vector2f getPoint(size_t index) const override {
			return m_points[index];
		}

		void setSize(vec2 size) {
			m_size = size;
			updatePoints();
		}

		void setRadius(float radius) {
			m_radius = radius;
			updatePoints();
		}

		void set(vec2 size, float radius) {
			m_size = size;
			m_radius = radius;
			updatePoints();
		}
	};
} // namespace flx::ui