#pragma once
#include "src/ecs/dense_map.h"
#include "src/ecs/types.h"
#include "src/game/Contact/ContactLayerRules.h"
#include "src/game/Spells/SpellBlock.h"
#include "src/meta/MetaAttributes.h"
#include "src/utils/Pointer.h"
#include "src/utils/Timer.h"
#include "src/utils/Vec2.h"

struct EntityComponent {
	ContactLayer layer = ContactLayer::None;
	int health = -1;
};

N_NOINIT struct MaterialComponent {
	int value = 1;
};

N_NOINIT struct LifetimeComponent {
	Timer lifeTimer;
};

// need EntityComponent, BodyComponent
// struct ContactPrefComponent {
//	enum : char {
//		NoPref = 0,
//		Enabled = 2,
//		Disabled = 1,
//	};
//
//	std::array<char, ContactManager::max_type_size> contactPref = {};	//init as NoPref
//
//	void setContactPref(ContactLayer t, bool enabled) {
//		contactPref[t] = enabled ? Enabled : Disabled;
//	}
//
// };

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

	N_NOINIT n_shared<ProjectileSpell> spell; // todo change all these pointers to raw pointer
	N_NOINIT std::vector<n_shared<ModifierSpell>> mods;
};

// mark an enemy
struct EnemyComponent {
	N_NOINIT nvec2 impulse; // impulse recieved during last iteration, used for death animation
};

struct DirectionComponent {
	N_NOINIT nvec2 dir; // The dir this entity intends to go. Resets each frame.
};

N_NOINIT struct SpellOnDeathComponent {
	SpellBlock spellBlock;
	nvec2 impulseDir;
	nvec2 impulsePosFix;
};

// for piercing entites and explosions
// maybe use std pmr (so I think it can be 10x faster)
struct MultiContactComponent {
	float default_duration = 0.1f;
	N_NOINIT myecs::DenseMap<myecs::entity, Timer> banned;
};