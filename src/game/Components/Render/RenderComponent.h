#pragma once
#include "../../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

namespace sf {
	class Sprite;
}

namespace flx::game {
	struct SpriteData;

	struct SpriteRenderOptions {
		bool followPosition = true;
		bool followRotation = true;
		bool dynamicScale{};
		float rotation{}; // As degree
		vec2 offset{};
		bool centerAlinged = true;
		vec2 scale = {1.f, 1.f};
		vec2 targetSize{}; // 可选，与scale同时生效
	};

	enum class RenderLayer {
		Bottom,
		Enemey,
		Player,
		PlayerProjectile,
		EnemyProjectile,
		Top
	};

	struct SpriteComponent {
		const sf::Texture& texture;
		const SpriteRenderOptions& options;
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
		Vector<Unique<BaseEffect>> effectList;
	};
} // namespace flx::game