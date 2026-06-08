#pragma once

#include "src/utils/Container/Vector.h"
#include "src/utils/Integers.h"
#include <initializer_list>
#include <utility>

enum class ContactLayer : u64 {
	None,
	All,
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
	static constexpr u64 MAX_LAYER_COUNT = static_cast<const u64>(ContactLayer::ContactLayerCount);
	static_assert(MAX_LAYER_COUNT <= 63, "too many types!");

	using Mask = u64;
	using Rules = Util::Array<Mask, MAX_LAYER_COUNT>;

	Rules rules{};

	static constexpr Mask bit(ContactLayer layer) noexcept {
		return Mask{1} << static_cast<u64>(layer);
	}

	static constexpr Mask validLayerBits() noexcept {
		return ((Mask{1} << MAX_LAYER_COUNT) - Mask{1}) & ~bit(ContactLayer::None);
	}

	void sanitizeNone() noexcept {
		rules[ContactLayer::None] = Mask{0};

		for (auto& mask : rules) {
			mask &= ~bit(ContactLayer::None);
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
		preSolve.set(ContactLayer::Wall, true);
		preSolve.set(ContactLayer::Wall, ContactLayer::Wall, false);

		preSolve.set(ContactLayer::Collector, ContactLayer::Wall, false);

		preSolve.set(ContactLayer::Collectible, false);
		preSolve.set(ContactLayer::Collector, ContactLayer::Collectible);
		preSolve.set(ContactLayer::Collectible, ContactLayer::Collectible);
		preSolve.set(ContactLayer::Collectible, ContactLayer::Wall);

		preSolve.set(ContactLayer::Detector, true);
		preSolve.set(ContactLayer::Detector, ContactLayer::Detector, false);

		preSolve.set(ContactLayer::All, true);

		std::initializer_list<std::pair<ContactLayer, ContactLayer>> contacts = {
			{ContactLayer::Player, ContactLayer::Enemy},
			{ContactLayer::Player, ContactLayer::Projectile},
			{ContactLayer::Player, ContactLayer::EnemyProjectile},
			{ContactLayer::Enemy, ContactLayer::Projectile},
			{ContactLayer::Enemy, ContactLayer::PlayerProjectile},
			{ContactLayer::Enemy, ContactLayer::Enemy},
		};

		for (const auto& [a, b] : contacts) {
			preSolve.set(a, b, true);
		}

		physics.set(ContactLayer::Wall, ContactLayer::Player);
		physics.set(ContactLayer::Wall, ContactLayer::Enemy);
		physics.set(ContactLayer::Wall, ContactLayer::Collectible);

		softContact.set(ContactLayer::Enemy, ContactLayer::Enemy);
		softContact.set(ContactLayer::Player, ContactLayer::Enemy);
	}

	static constexpr u64 bit(ContactLayer layer) noexcept {
		return LayerRules::bit(layer);
	}
};