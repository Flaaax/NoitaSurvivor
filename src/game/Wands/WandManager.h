#pragma once
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"

namespace flx::ui {
	class Painter;
}

namespace flx::game {
	struct GameCtx;
	class Wand;

	class WandManager {
	private:
		Vector<SUnique<Wand>> wands;
		bool shouldRecalcArg{};
		void updateGeometry(const GameCtx& ctx, float dt);

	public:
		~WandManager();
		void update(const GameCtx& ctx, float dt);
		void draw(const ui::Painter& painter) const;
		SWeak<Wand> getWand(u64 i) const;
		SWeak<Wand> addWand(float scale);
		Vector<SWeak<Wand>> getWands()const;
		void clear();

		u64 count() const;
	};
} // namespace flx::game
