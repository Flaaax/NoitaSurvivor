#pragma once
#include "src/ecs/types.h"
#include <boost/container_hash/hash.hpp>

struct EntityPair {
	myecs::entity a{};
	myecs::entity b{};

	constexpr EntityPair() = default;

	constexpr EntityPair(myecs::entity a, myecs::entity b) noexcept :
		a(a._entity <= b._entity ? a : b),
		b(a._entity <= b._entity ? b : a) {}

	friend bool operator==(const EntityPair&, const EntityPair&) = default;

	friend size_t hash_value(const EntityPair& pair) noexcept {
		size_t seed = 0;
		boost::hash_combine(seed, pair.a._entity);
		boost::hash_combine(seed, pair.b._entity);
		return seed;
	}
};
