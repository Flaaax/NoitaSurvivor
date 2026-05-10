#pragma once
#include"src/utils/Container.h"
#include"src/utils/Vec2.h"


struct SpriteData;

namespace sf {
	class Sprite;
}

struct SpriteInfo {
	bool followPosition{};
	bool followAngle{};
	bool dynamicScale{};
	float rotationOffset{};		//As degree
	nvec2 positionOffset{};
};

struct SpriteComponent {
	const SpriteInfo* info;
	sf::Sprite* sprite;
};

class BaseEffect {
private:
	bool m_isDone = false;

protected:
	void done() {
		m_isDone = true;
	}

public:

	virtual void apply(sf::Sprite& sprite)const {}
	virtual void update(float dt) {}

	virtual ~BaseEffect() {}
	bool isDone()const { return m_isDone; }

};

//requires SpriteComponent
struct SpriteEffectComponent {
	Util::Vector<n_unique<BaseEffect>> effectList;
};
