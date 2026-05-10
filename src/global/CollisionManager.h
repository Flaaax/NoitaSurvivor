#pragma once
#include<array>
#include"src/utils/Singleton.h"


//enum EntityType {
//	PLAYER,
//	ENEMY,
//	ENEMY_PROJECTILE,
//	PLAYER_PROJECTILE,
//	ALL_DMG_PROJECTILE,
//	CUSTOM_PROJECTILE,
//	BORDER,
//	WALL,
//	UNKNOWN,
//	INVALID
//};
//
//enum CollidePref {
//	NONE = 0,
//	ALLOWED,
//	FORBIDDEN
//};
//
//enum ContactType {
//	PIERCE,
//	RICOCHET,
//	NORMAL
//};
//
//struct CollisionPair {
//	EntityType a, b;
//
//	CollisionPair() = delete;
//	CollisionPair(EntityType a, EntityType b) :a(std::min(a, b)), b(std::max(a, b)) {}
//
//	bool operator==(const CollisionPair& other) const {
//		return a == other.a && b == other.b;
//	}
//
//	struct Hash {
//		std::size_t operator()(const CollisionPair& pair) const {
//			return static_cast<std::size_t>(pair.a) ^ static_cast<std::size_t>(pair.b);
//		}
//	};
//};
//
//class CollisionManager : public Singleton<CollisionManager> {
//private:
//	friend class Singleton<CollisionManager>;
//	using Type = EntityType;
//	using CollisionSet = std::unordered_set<CollisionPair, CollisionPair::Hash>;
//	using AlwaysColliders = std::unordered_set<Type>;
//
//	CollisionSet collisionSet;
//	AlwaysColliders alwaysCollidersSet;
//
//	CollisionManager() {
//		addPairs({
//				 {Type::PLAYER,Type::ENEMY},
//				 {Type::PLAYER,Type::ENEMY_PROJECTILE},
//				 {Type::ENEMY,Type::ENEMY},
//				 {Type::ENEMY,Type::PLAYER_PROJECTILE},
//				 });
//
//		addAlwaysCollider(Type::WALL);
//		addAlwaysCollider(Type::BORDER);
//		addAlwaysCollider(Type::ALL_DMG_PROJECTILE);
//	}
//
//public:
//	inline void addPair(const CollisionPair& pair) {
//		collisionSet.insert(pair);
//	}
//
//	inline void addPairs(const std::vector<CollisionPair>& pairs) {
//		for (auto& pair : pairs) {
//			collisionSet.insert(pair);
//		}
//	}
//
//	inline void addAlwaysCollider(Type a) {
//		alwaysCollidersSet.insert(a);
//	}
//
//	bool canCollide(const CollisionPair& pair)const {
//		if (alwaysCollidersSet.count(pair.a) || alwaysCollidersSet.count(pair.b)) {
//			return true;
//		}
//		if (collisionSet.count(pair)) {
//			return true;
//		}
//		return false;
//	}
//
//	bool canCollide(Type a, Type b)const {
//		return canCollide({ a,b });
//	}
//};


enum EntityType :size_t {
	None,
	Player,
	Enemy,
	Projectile,			//hits player and enemy
	PlayerProjectile,
	EnemyProjectile,
	Wall
};


class ContactManager {
private:
	static constexpr size_t max_type_size = 10;
	using contact_info = std::array<std::array<bool, max_type_size>, max_type_size>;

	contact_info m_contacts = { {} };

	struct _ContactArg {
		EntityType a, b;
		bool enabled;
		_ContactArg(EntityType a, EntityType b, bool enabled = true) :a(a), b(b), enabled(enabled) {}
	};


	ContactManager() {
		setContact(Wall, true);
		std::initializer_list<_ContactArg> contacts = {
					{Player,Enemy},
					{Player,Projectile},
					{Player,EnemyProjectile},
					{Enemy,Projectile},
					{Enemy,PlayerProjectile}
		};
		setContacts(contacts);
	}

public:
	void setContacts(std::initializer_list<_ContactArg> contacts) {
		for (auto& contact : contacts) {
			setContact(contact.a, contact.b, contact.enabled);
		}
	}

	void setContact(EntityType a, EntityType b, bool enabled) {
		//a>=b
		if (b < a) {
			std::swap(a, b);
		}
		m_contacts[a][b] = enabled;
	}

	void setContact(EntityType t, bool enabled) {
		for (size_t i = 0; i < m_contacts.size(); i++) {
			setContact(t, static_cast<EntityType>(i), enabled);
		}
	}

	bool shouldContact(EntityType a, EntityType b) {
		if (b < a) {
			std::swap(a, b);
		}
		return m_contacts[a][b];
	}
};