#include "RoundRectShape.h"
#include "src/utils/Math.h"

namespace flx::ui {
	void RoundRectShape::updatePoints() {
		m_points.resize(m_segments * 4);

		size_t index = 0;
		const float step = (math::pi / 2.f) / static_cast<float>(m_segments - 1);
		for (size_t corner = 0; corner < 4; corner++) {
			vec2 cornerPos;
			vec2 cornerEnd;
			switch (corner) {
			case 0:
				cornerPos = {m_size.x - m_radius, m_size.y - m_radius};
				cornerEnd = cornerPos + vec2{0, m_radius};
				break;
			case 1:
				cornerPos = {m_radius, m_size.y - m_radius};
				cornerEnd = cornerPos + vec2{-m_radius, 0};
				break;
			case 2:
				cornerPos = {m_radius, m_radius};
				cornerEnd = cornerPos + vec2{0, -m_radius};
				break;
			case 3:
				cornerPos = {m_size.x - m_radius, m_radius};
				cornerEnd = cornerPos + vec2{m_radius, 0};
				break;
			default:
				break;
			}
			for (size_t i = 0; i < m_segments - 1; i++) {
				const float angle = corner * (math::pi / 2.f) + i * step;
				const float x = m_radius * std::cos(angle);
				const float y = m_radius * std::sin(angle);
				m_points[index++] = cornerPos + vec2{x, y};
			}
			m_points[index++] = cornerEnd;
		}

		Shape::update();
	}
} // namespace flx::ui