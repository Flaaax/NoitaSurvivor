#pragma once

#include "src/utils/Container/Vector.h"
#include "src/utils/FlagSet.h"
#include "src/utils/Integers.h"
#include <initializer_list>
#include <utility>

namespace flx::game {
	enum class EntityType : u64 {
		None,
		All,
		Player,
		Enemy,
		Proj, // hits player and enemy
		PlayerProj,
		EnemyProj,
		Wall,
		Collectible,
		Collector,
		Detector,

		ContactLayerCount
	};

	struct LayerRules {
		static constexpr u64 MAX_LAYER_COUNT = static_cast<const u64>(EntityType::ContactLayerCount);
		static_assert(MAX_LAYER_COUNT <= 63, "too many types!");

		using Bits = u64;
		using Mask = EnumSet<EntityType, MAX_LAYER_COUNT>;
		using Rules = Array<Mask, MAX_LAYER_COUNT>;

		Rules rules{};

		static constexpr Bits bit(EntityType layer) noexcept {
			return Mask::bit(layer);
		}

		void sanitizeNone() noexcept {
			rules[EntityType::None] = {};

			for (auto& mask : rules) {
				mask.remove(EntityType::None);
			}
		}

		void set(EntityType a, bool enabled) noexcept {
			rules[a] = Mask::all(enabled);

			for (auto& mask : rules) {
				mask.set(a, enabled);
			}

			sanitizeNone();
		}

		void set(EntityType a, EntityType b, bool enabled = true) noexcept {
			rules[a].set(b, enabled);
			rules[b].set(a, enabled);

			sanitizeNone();
		}

		void setAll(bool enabled) noexcept {
			for (auto& rule : rules) {
				rule = Mask::all(enabled);
			}

			sanitizeNone();
		}

		bool get(EntityType a, EntityType b) const noexcept {
			return rules[a].has(b);
		}

		Bits getCollisionBits(EntityType a) const noexcept {
			return rules[a].flatten();
		}
	};

	struct ContactLayerRules {
		LayerRules preSolve{};
		LayerRules physics{};
		LayerRules softContact{};

		ContactLayerRules() {
			preSolve.set(EntityType::Wall, true);
			preSolve.set(EntityType::Wall, EntityType::Wall, false);

			preSolve.set(EntityType::Collector, EntityType::Wall, false);

			preSolve.set(EntityType::Collectible, false);
			preSolve.set(EntityType::Collector, EntityType::Collectible);
			preSolve.set(EntityType::Collectible, EntityType::Collectible);
			preSolve.set(EntityType::Collectible, EntityType::Wall);

			preSolve.set(EntityType::Detector, true);
			preSolve.set(EntityType::Detector, EntityType::Detector, false);

			preSolve.set(EntityType::All, true);

			std::initializer_list<std::pair<EntityType, EntityType>> contacts = {
				{EntityType::Player, EntityType::Enemy},
				{EntityType::Player, EntityType::Proj},
				{EntityType::Player, EntityType::EnemyProj},
				{EntityType::Enemy, EntityType::Proj},
				{EntityType::Enemy, EntityType::PlayerProj},
				{EntityType::Enemy, EntityType::Enemy},
			};

			for (const auto& [a, b] : contacts) {
				preSolve.set(a, b, true);
			}

			physics.set(EntityType::Wall, EntityType::Player);
			physics.set(EntityType::Wall, EntityType::Enemy);
			physics.set(EntityType::Wall, EntityType::Collectible);

			softContact.set(EntityType::Enemy, EntityType::Enemy);
			softContact.set(EntityType::Player, EntityType::Enemy);
		}

		static constexpr u64 bit(EntityType layer) noexcept {
			return LayerRules::bit(layer);
		}
	};
} // namespace flx::game
