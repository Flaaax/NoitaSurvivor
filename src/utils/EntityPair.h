#pragma once
#include "src/ecs/types.h"
#include <boost/container_hash/hash.hpp>

struct EntityPair {
	myecs::entity a{};
	myecs::entity b{};

	constexpr EntityPair() = default;

	constexpr EntityPair(myecs::entity a, myecs::entity b) noexcept :
		a(a.flatten() <= b.flatten() ? a : b),
		b(a.flatten() <= b.flatten() ? b : a) {}

	friend bool operator==(const EntityPair&, const EntityPair&) = default;

	friend size_t hash_value(const EntityPair& pair) noexcept {
		size_t seed = 0;
		boost::hash_combine(seed, pair.a.flatten());
		boost::hash_combine(seed, pair.b.flatten());
		return seed;
	}
};
