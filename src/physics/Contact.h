#pragma once
#ifndef N_CONTACT_H
#define N_CONTACT_H
#include"Body.h"


namespace physics {
	struct Manifold {

	};

	//A contact occurs when two AABB intersects
	struct Contact {
		Body* a;
		Body* b;

		Manifold manifolds[2];

		Contact(Body* a, Body* b) {
			if (a->m_type > b->m_type) {
				this->a = b;
				this->b = a;
			}
			else {
				this->a = a;
				this->b = b;
			}
		}
	};

	class ContactSolver {
	private:
		int rectangleCollide(Contact* contact);

	public:

		int solve(Contact* contact);
	};

}


#endif