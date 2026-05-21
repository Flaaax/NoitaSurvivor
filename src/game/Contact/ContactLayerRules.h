#pragma once

#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"
#include <initializer_list>
#include <utility>

enum ContactLayer : u64 {
	None,
	Player,
	Enemy,
	Projectile, // hits player and enemy
	PlayerProjectile,
	EnemyProjectile,
	Wall,
	Collectible,
	Collector,
	Detector,

	ContactLayerCount
};

struct LayerRules {
	static constexpr u64 MAX_LAYER_COUNT = ContactLayerCount;
	static_assert(MAX_LAYER_COUNT <= 63, "too many types!");

	using Mask = u64;
	using Rules = Util::Array<Mask, MAX_LAYER_COUNT>;

	Rules rules{};

	static constexpr Mask bit(ContactLayer layer) noexcept {
		return Mask{1} << layer;
	}

	static constexpr Mask validLayerBits() noexcept {
		return ((Mask{1} << MAX_LAYER_COUNT) - Mask{1}) & ~bit(None);
	}

	void sanitizeNone() noexcept {
		rules[None] = Mask{0};

		for (auto& mask : rules) {
			mask &= ~bit(None);
		}
	}

	void set(ContactLayer a, bool enabled) noexcept {
		const Mask aBit = bit(a);

		if (enabled) {
			rules[a] = validLayerBits();

			for (auto& mask : rules) {
				mask |= aBit;
			}
		} else {
			rules[a] = Mask{0};

			for (auto& mask : rules) {
				mask &= ~aBit;
			}
		}

		sanitizeNone();
	}

	void set(ContactLayer a, ContactLayer b, bool enabled = true) noexcept {
		const Mask aBit = bit(a);
		const Mask bBit = bit(b);

		if (enabled) {
			rules[a] |= bBit;
			rules[b] |= aBit;
		} else {
			rules[a] &= ~bBit;
			rules[b] &= ~aBit;
		}

		sanitizeNone();
	}

	void setAll(bool enabled) noexcept {
		const Mask mask = enabled ? validLayerBits() : Mask{0};

		for (auto& rule : rules) {
			rule = mask;
		}

		sanitizeNone();
	}

	bool get(ContactLayer a, ContactLayer b) const noexcept {
		return (rules[a] & bit(b)) != 0;
	}

	Mask getCollisionBits(ContactLayer a) const noexcept {
		return rules[a];
	}
};

struct ContactLayerRules {
	LayerRules preSolve{};
	LayerRules physics{};
	LayerRules softContact{};

	ContactLayerRules() {
		preSolve.set(Wall, true);
		preSolve.set(Wall, Wall, false);

		preSolve.set(Collector, Wall, false);

		preSolve.set(Collectible, false);
		preSolve.set(Collector, Collectible);
		preSolve.set(Collectible, Collectible);
		preSolve.set(Collectible, Wall);

		preSolve.set(Detector, true);
		preSolve.set(Detector, Detector, false);

		std::initializer_list<std::pair<ContactLayer, ContactLayer>> contacts = {
			{Player, Enemy},
			{Player, Projectile},
			{Player, EnemyProjectile},
			{Enemy, Projectile},
			{Enemy, PlayerProjectile},
			{Enemy, Enemy},
		};

		for (const auto& [a, b] : contacts) {
			preSolve.set(a, b, true);
		}

		physics.set(Wall, Player);
		physics.set(Wall, Enemy);
		physics.set(Wall, Collectible);

		softContact.set(Enemy, Enemy);
		softContact.set(Player, Enemy);
	}

	static constexpr uint64_t bit(ContactLayer layer) noexcept {
		return u64{1} << layer;
	}
};