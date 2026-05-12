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

	LayerRules m_rules{};

	struct ContactLayerPair {
		ContactLayer a;
		ContactLayer b;
		bool enabled = true;
	};

	void setContacts(std::initializer_list<ContactLayerPair> contacts) {
		for (const auto& [a, b, enabled] : contacts) {
			setContact(a, b, enabled);
		}
	}

public:
	ContactLayerRules() {
		setContact(Wall, true);
		setContact(Wall, Wall, false);

		setContact(Collector, Wall, false);

		setContact(Collectible, false);
		setContact(Collector, Collectible, true);
		setContact(Collectible, Collectible, true);

		setContact(Detector, true);
		setContact(Detector, Detector, false);

		setContacts({
			{Player, Enemy},
			{Player, Projectile},
			{Player, EnemyProjectile},
			{Enemy, Projectile},
			{Enemy, PlayerProjectile},
			{Enemy, Enemy},
		});
	}

	// Should only be called during game init.
	// This function preserves the symmetric invariant:
	// m_contacts[a][b] == m_contacts[b][a]
	void setContact(ContactLayer a, ContactLayer b, bool enabled) {
		if (a == None || b == None)
			return;

		m_rules[a][b] = enabled;
		m_rules[b][a] = enabled;
	}

	void setContact(ContactLayer t, bool enabled) {
		if (t == None)
			return;

		for (size_t i = 0; i < m_rules.size(); ++i) {
			setContact(t, static_cast<ContactLayer>(i), enabled);
		}
	}

	bool shouldContact(ContactLayer a, ContactLayer b) const { return m_rules[a][b]; }
};