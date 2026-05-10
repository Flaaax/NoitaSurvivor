#pragma once
#include<array>


enum ContactLayer : size_t {
	None,
	Player,
	Enemy,
	Projectile,			//hits player and enemy
	PlayerProjectile,
	EnemyProjectile,
	Wall,
	Collectable
};

using EntityType = ContactLayer;

enum class ContactMaskFilter : unsigned short {
	Default = 1 << 0,
	Collectable = 1 << 1,
};


class ContactLayerRules {
public:
	static constexpr size_t max_type_size = 10;
private:
	using contact_info = std::array<std::array<bool, max_type_size>, max_type_size>;

	contact_info m_contacts{};

	struct _ContactArg {
		ContactLayer a, b;
		bool enabled;
		_ContactArg(ContactLayer a, ContactLayer b, bool enabled = true) :a(a), b(b), enabled(enabled) {}
	};

	void setContacts(std::initializer_list<_ContactArg> contacts) {
		for (auto& contact : contacts) {
			setContact(contact.a, contact.b, contact.enabled);
		}
	}
public:
	ContactLayerRules() {
		for (auto& i : m_contacts) {
			for (auto& j : i) {
				j = false;
			}
		}
		setContact(Wall, true);
		setContact(Wall, Wall, false);
		setContact(Collectable, Collectable, true);
		std::initializer_list<_ContactArg> contacts = {
					{Player,Enemy},
					{Player,Projectile},
					{Player,EnemyProjectile},
					{Enemy,Projectile},
					{Enemy,PlayerProjectile},
					{Enemy,Enemy},
		};
		setContacts(contacts);
	}

	void setContact(ContactLayer a, ContactLayer b, bool enabled) {
		if (a == None || b == None)return;
		//a>=b
		if (b < a) {
			std::swap(a, b);
		}
		m_contacts[a][b] = enabled;
	}

	void setContact(ContactLayer t, bool enabled) {
		for (size_t i = 0; i < m_contacts.size(); i++) {
			setContact(t, static_cast<ContactLayer>(i), enabled);
		}
	}

	bool shouldContact(ContactLayer a, ContactLayer b) {
		if (b < a) {
			std::swap(a, b);
		}
		return m_contacts[a][b];
	}
};