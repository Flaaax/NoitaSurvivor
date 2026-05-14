#pragma once
#include <array>
#include <initializer_list>

enum ContactLayer : size_t {
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

class ContactLayerRules {
public:
	static constexpr size_t MAX_LAYER_COUNT = ContactLayerCount;
	static_assert(MAX_LAYER_COUNT <= 16, "too many types!");

private:
	using LayerRules = std::array<std::array<bool, MAX_LAYER_COUNT>, MAX_LAYER_COUNT>;

	LayerRules rules{};
	LayerRules physicsRules{};

public:
	ContactLayerRules() {
		for (auto& b1 : physicsRules) {
			for (auto& b : b1) {
				b = true;
			}
		}

		setContact(Wall, true);
		setContact(Wall, Wall, false);

		setContact(Collector, Wall, false);

		setContact(Collectible, false);
		setContact(Collector, Collectible, true);
		setContact(Collectible, Collectible, true);
		setContact(Collectible, Wall, true);

		setContact(Detector, true);
		setContact(Detector, Detector, false);

		std::initializer_list<std::pair<ContactLayer, ContactLayer>> contacts = {
			{Player, Enemy},
			{Player, Projectile},
			{Player, EnemyProjectile},
			{Enemy, Projectile},
			{Enemy, PlayerProjectile},
			{Enemy, Enemy},
		};
		for (const auto& [a, b] : contacts) {
			setContact(a, b, true);
		}

		setPhysicsContact(Projectile, false);
		setPhysicsContact(PlayerProjectile, false);
		setPhysicsContact(EnemyProjectile, false);
		setPhysicsContact(Collectible, false);
		setPhysicsContact(Collectible, Collectible, true);
		setPhysicsContact(Wall, Collectible, true);
	}

	// Should only be called during game init.
	// This function preserves the symmetric invariant:
	// m_contacts[a][b] == m_contacts[b][a]
	void setContact(ContactLayer a, ContactLayer b, bool enabled) {
		if (a == None || b == None)
			return;

		rules[a][b] = enabled;
		rules[b][a] = enabled;
	}

	void setContact(ContactLayer t, bool enabled) {
		if (t == None)
			return;

		for (size_t i = 0; i < rules.size(); ++i) {
			setContact(t, static_cast<ContactLayer>(i), enabled);
		}
	}

	void setPhysicsContact(ContactLayer a, ContactLayer b, bool enabled) {
		if (a == None || b == None)
			return;

		physicsRules[a][b] = enabled;
		physicsRules[b][a] = enabled;
	}

	void setPhysicsContact(ContactLayer t, bool enabled) {
		if (t == None)
			return;

		for (size_t i = 0; i < rules.size(); ++i) {
			setPhysicsContact(t, static_cast<ContactLayer>(i), enabled);
		}
	}

	bool shouldContact(ContactLayer a, ContactLayer b) const {
		return rules[a][b];
	}

	bool shouldContactPhysics(ContactLayer a, ContactLayer b) const {
		return physicsRules[a][b];
	}
};