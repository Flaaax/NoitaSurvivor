#pragma once
#include "../../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

namespace sf {
	class Sprite;
}

namespace flx::game {
	struct SpriteData;

	struct SpriteInfo {
		bool followPosition{};
		bool followAngle{};
		bool dynamicScale{};
		float rotationOffset{}; // As degree
		vec2 positionOffset{};
	};

	struct SpriteComponent {
		const SpriteInfo& info;
		const sf::Sprite& sprite;
		vec2 position{}; // used when entity does not have body
	};

	class BaseEffect {
	private:
		bool m_isDone = false;

	protected:
		void done() {
			m_isDone = true;
		}

	public:
		virtual void apply(sf::Sprite& sprite) const {}

		virtual void update(float dt) {}

		virtual ~BaseEffect() {}

		bool isDone() const {
			return m_isDone;
		}
	};

	// requires SpriteComponent
	struct SpriteEffectComponent {
		Vector<n_unique<BaseEffect>> effectList;
	};
} // namespace flx::game