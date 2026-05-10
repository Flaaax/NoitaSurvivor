#pragma once
#ifndef N_BODY_H
#define N_BODY_H
#include"src/utils/Vec2.h"
#include"src/utils/Assert.h"
#pragma warning(push)
#pragma warning(disable: 26495)


namespace physics {

	class Body {
		friend class World;
		friend class Contact;
		friend class ContactSolver;
	public:
		enum {
			Dynamic,
			Static,
		};
		enum {
			None = 0,
			Polygon = 1,
			Circle = 2,
			Line = 3
		};

	private:
		nvec2 m_p;
		nvec2 m_a;
		nvec2 m_v;
		union {
			struct {	//Circle
				float m_radius;
			};
			struct {	//Line
				nvec2 m_end;
			};
			struct {	//Polygon
				nvec2 m_size;
				float m_rotation;
				float m_sin;
				float m_cos;
			};
		};

		int m_type = None;
		bool m_enabled = true;

		nrect getCircleAABB()const {
			return nrect(m_p.x, m_p.y, m_radius, m_radius, nrect::CenterRect);
		}


	public:
		Body() {}

		void setEnable(bool enabled) {
			m_enabled = enabled;
		}

		bool enabled()const {
			return m_enabled;
		}

		void setPosition(const nvec2& position) {
			m_p = position;
		}

		const nvec2& getPosition()const {
			return m_p;
		}

		void setRectangle(const nvec2& size, float rotation = 0) {
			MYASSERT(size.isNonNegative(), "size must be non negative!");
			m_type = Polygon;
			m_size = size;
			m_rotation = rotation;
			m_sin = std::sin(rotation);
			m_cos = std::cos(rotation);
		}

		void setCircle(float radius) {
			MYASSERT(radius >= 0, "radius must be non negative");
			m_type = Circle;
			m_radius = radius;
		}

		void setLine(const nvec2& start, const nvec2& end) {
			m_type = Polygon;
			m_p = start;
			m_end = end;
		}

		//should be called once per iteration
		nrect getAABB()const {
			if (m_type == Rectangle) {
				return getRectangleAABB();
			}
			else if (m_type == Circle) {
				return getCircleAABB();
			}
			else if (m_type == Line) {
				return getLineAABB();
			}
			MYASSERT(false, "type of the body is None");
			return {};
		}


	};

}


#pragma warning(pop)
#endif