#pragma once
#include "src/ui/elements/Panel.h"

namespace flx::game {
	class WandManager;
	class Wand;
}

namespace flx::ui {
	class WandEditor : public Panel {
	protected:
		Vector<SWeak<game::Wand>> wands;

	public:
		WandEditor() = default;
		void setWands(const game::WandManager& manager);
	};

} // namespace flx::ui