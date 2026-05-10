#pragma once

#ifndef N_WORLD_H
#define N_WORLD_H
#include"Body.h"
#include"Allocator.h"
#include"Contact.h"


namespace physics {

	class World {
	private:
		BodyAllocator m_bodyAllocator;
		std::vector<Contact> m_contacts;
		bool m_locked = false;

		void solve(float dt, size_t iterations);
		int collide(Contact* contact);

	public:

		body_id createBody() {
			return m_bodyAllocator.create();
		}

		void destroyBody(body_id id) {
			m_bodyAllocator.destroy(id);
		}

		Body& getBody(body_id id) {
			return m_bodyAllocator.get(id);
		}

		void step(float dt, size_t iterations);
	};

}


#endif