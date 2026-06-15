#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/ecs/dense_map.h"
#include "src/ecs/types.h"
#include "src/game/Contact/ContactLayerRules.h"
#include "src/game/Spells/SpellBlock.h"
#include "src/meta/MetaAttributes.h"
#include "src/utils/Pointer.h"
#include "src/utils/Timer.h"

namespace flx::game {
	struct EntityComponent {
		// Do not modify this after initialized
		ContactLayer layer = ContactLayer::None;
		int health = -1;
	};

	N_NOINIT struct DieSilentComponent {};

	N_NOINIT struct MaterialComponent {
		int value = 1;
	};

	N_NOINIT struct LifetimeComponent {
		Timer lifeTimer;
	};

	class b2Contact;
	class ProjectileSpell;
	class ModifierSpell;

	// need BodyComponent, EntityComponent
	struct ProjectileComponent {
		int damage = 0;
		float impulse = 0;
		float maxSpeed = 35.f;

		int pierce = 0; //-1 = always pierce

		// Should disable physical contact and give impulse manually
		bool isBullet = true;

		N_NOINIT Shared<ProjectileSpell> spell; // todo change all these pointers to raw pointer
		N_NOINIT std::vector<Shared<ModifierSpell>> mods;
	};

	// marks an enemy
	struct EnemyComponent {
		N_NOINIT vec2 impulseRecieved; // impulse recieved during last iteration, used for death animation
	};

	struct DirectionComponent {
		N_NOINIT vec2 dir; // The dir this entity intends to go. Resets each frame.
	};

	N_NOINIT struct SpellOnDeathComponent {
		SpellBlock spellBlock;
		vec2 impulseDir;
		vec2 impulsePosFix;
	};

	// for piercing entites and explosions
	// maybe use std pmr (so I think it can be 10x faster)
	struct MultiContactComponent {
		float default_duration = 0.1f;
		N_NOINIT myecs::DenseMap<myecs::entity, Timer> banned;
	};

	// Marks an explosion
	struct ExplosionComponent {
	};

	N_NOINIT struct EnemyBirthComponent {
		vec2 position{};
		std::string name{};
		float remainingTime = 1.f;
	};
} // namespace flx::game