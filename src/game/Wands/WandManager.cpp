#include "WandManager.h"

#include "Wand.h"
#include "src/game/GameContext.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/utils/Math.h"

namespace flx::game {

	WandManager::~WandManager() {
	}

	void WandManager::update(const GameCtx& ctx, float dt) {
		updateGeometry(ctx, dt);
		for (const auto& wand : wands) {
			wand->update(dt);

			if ((wand->autoAim && wand->target) ||
				((!wand->autoAim) && ctx.gameState.player.isShooting)) {
				wand->use(ctx);
			}
		}
	}

	void WandManager::draw(const ui::Painter& painter) const {
		for (auto& wand : wands) {
			wand->draw(painter);
		}
	}

	SWeak<Wand> WandManager::getWand(u64 i) const {
		return wands.at(i);
	}

	SWeak<Wand> WandManager::addWand(float scale) {
		shouldRecalcArg = true;
		return wands.emplace_back(makeSUnique<Wand>(scale));
	}

	void WandManager::clear() {
		wands.clear();
	}

	u64 WandManager::count() const {
		return wands.size();
	}

	void WandManager::updateGeometry(const GameCtx& ctx, float dt) {
		if (shouldRecalcArg && count() != 0) {
			float radius = 1.f;
			float arg{};
			const float off = 2.f * math::pi / static_cast<float>(count());

			if (count() == 1) {
				radius = 0;
			} else if (count() > 2 && count() % 2 == 0) {
				arg = -off / 2.f;
			} else if (count() % 2 == 1) {
				arg = -math::pi / 2.f;
			}

			for (const auto i : wands.indices<int>()) {
				const auto& wand = wands[i];
				wand->localPos = radius * vec2::rad(arg);
				arg += off;
			}
		}

		shouldRecalcArg = false;

		for (const auto& wand : wands) {
			wand->updateTarget(ctx);
			wand->updateGeometry(dt);
		}
	}
} // namespace flx::game