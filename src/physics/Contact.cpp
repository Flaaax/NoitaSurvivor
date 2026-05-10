#include"Contact.h"
#include"src/utils/Mat22.h"


namespace physics {

    int ContactSolver::rectangleCollide(Contact* contact) {
		//SAT theorem

		Body* a = contact->a;
		Body* b = contact->b;

		nvec2 ha = a->m_size * 0.5f;
		nvec2 hb = b->m_size * 0.5f;

		nmat22 rotA = { a->m_cos,-a->m_sin,a->m_sin,a->m_cos };
		nmat22 rotB = { b->m_cos,-b->m_sin,b->m_sin,b->m_cos };

		nmat22 rotAT = rotA.transpose();
		nmat22 rotBT = rotB.transpose();

		nvec2 dp = b->m_p - a->m_p;		//relavive position
		nvec2 dpa = rotAT * dp;			//dp in a coords
		nvec2 dpb = rotBT * dp;			//dp in b coords
		
		//transform B to A
		nmat22 C = rotAT * rotB;

		nmat22 absC = C.abs();
		nmat22 absCT = absC.transpose();

		// Box A faces
		nvec2 faceA = dpa.abs() - ha - absC * hb;
		if (faceA.x > 0.0f || faceA.y > 0.0f)
			return 0;

		// Box B faces
		nvec2 faceB = dpb.abs() - absCT * ha - hb;
		if (faceB.x > 0.0f || faceB.y > 0.0f)
			return 0;


    }

    int ContactSolver::solve(Contact* contact) {
        if (contact->a->m_type == Body::Rectangle && contact->b->m_type == Body::Rectangle) {
            return rectangleCollide(contact);
        }
    }

}