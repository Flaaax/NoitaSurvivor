#pragma once
#include "src/ecs/entity.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Math.h"
#include "src/utils/Pointer.h"
#include "src/utils/Timer.h"
#include "src/utils/Vec2/Vec2.h"

#include <SFML/Graphics/Sprite.hpp>
#include <deque>

namespace flx::ui {
	class Painter;
}

namespace flx::game {
	class Spell;
	struct GameCtx;

	class Wand {
	protected:
		sf::Sprite sprite;

		void reload();
		void cast(const GameCtx& ctx);
		static std::string getWandTextureEntry(int number);

	public:
		static constexpr float minCastDelay = 1.f / 60.f;

		// wand
		float castDelay = 0.f;
		float reloadDelay = 0.f;
		float scattering = 0.f;
		int capacity = 0;
		int castAmount = 0;

		bool isReloaded = true;

		float currentCastDelay = 0.f;
		float currentReloadDelay = 0.f;

		Timer castTimer;
		Timer reloadTimer;

		// gfx
		float targetRot{};
		float currentRot{};
		float length{};
		float holdLength{};

		vec2 localPos{};
		vec2 worldPos = {};
		vec2 castPos = {};

		Vector<Shared<Spell>> inventory; // Not modified when shooting
		Vector<Shared<Spell>> drawPile;
		Vector<Shared<Spell>> discardPile;
		Vector<Shared<Spell>> hand;

		float aimRadius = 8.f;
		float rotSpeed = math::pi / 0.15f;
		bool autoAim = true;
		myecs::entity target;

		explicit Wand(float wandScale);

		void updateGeometry(float dt);
		void updateTarget(const GameCtx& ctx);

		void draw(const ui::Painter& renderer) const;

		void update(float deltaTime) {
			castTimer.update(deltaTime);
			reloadTimer.update(deltaTime);
		}

		void use(const GameCtx& ctx) {
			if (!castTimer.isRunning() && !reloadTimer.isRunning()) {
				cast(ctx);
			}
		}

		// Clear all the spells but keep the delays
		void clear();
	};
} // namespace flx::game