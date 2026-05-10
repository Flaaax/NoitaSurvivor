#pragma once

#ifndef N_PHYSICS_ALLOCATOR_H
#define N_PHYSICS_ALLOCATOR_H
#include"Body.h"
#include"src/ecs/container.h"
#include<stack>


namespace physics {

	using body_id = size_t;

	class BodyAllocator {
	private:
		std::vector<Body> bodies;
		myecs::SparseSet<body_id> using_id;
		std::stack<body_id> avaliable_id;

	public:
		body_id create() {
			if (avaliable_id.empty()) {
				body_id new_id = bodies.size();
				bodies.emplace_back();
				using_id.insert(new_id);
				return new_id;
			}
			body_id id = avaliable_id.top();
			avaliable_id.pop();
			return id;
		}

		Body& get(body_id id) {
			MYASSERT(valid(id), "id is invalid!");
			return bodies[id];
		}

		void destroy(body_id id) {
			MYASSERT(valid(id), "id is invalid!");

			using_id.erase(id);
			avaliable_id.push(id);
		}

		bool valid(body_id id)const {
			return using_id.has(id);
		}

		const auto& view()const {
			return using_id;
		}
	};

}


#endif
