#include "World.h"

namespace physics {

	void World::step(float dt, size_t iterations) {
		//calculate AABBs and do initial steps

		std::vector<Body*> m_bodies;
		std::vector<nrect> m_AABBs;
		m_contacts.clear();

		const auto& body_ids = m_bodyAllocator.view();
		for (auto i : body_ids) {
			auto& body = m_bodyAllocator.get(i);
			if (!body.m_enabled) {
				continue;
			}
			if (body.m_type == Body::None) {
				continue;
			}
			m_bodies.emplace_back(&body);
			m_AABBs.emplace_back(body.getAABB());
		}
		for (size_t i = 0; i < m_bodies.size(); i++) {
			for (size_t j = i + 1; j < m_bodies.size(); j++) {
				if (m_AABBs[i].intersects(m_AABBs[j])) {
					m_contacts.emplace_back(m_bodies[i], m_bodies[j]);
					//do some callback?
				}
			}
		}

		solve(dt, iterations);
	}

	void World::solve(float dt, size_t iterations) {
		for (auto& contact : m_contacts) {

		}


	}

}