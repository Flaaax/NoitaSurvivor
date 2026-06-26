#include "WandEditor.h"

#include "SpellInventory.h"
#include "src/game/Wands/Wand.h"
#include "src/game/Wands/WandManager.h"
#include "src/ui/elements/Image.h"
#include "src/ui/layout/BoxLayout.h"

namespace flx::ui {

	void WandEditor::setWands(const game::WandManager& manager) {
		auto vlayout = std::make_unique<VBoxLayout>();
		vlayout->alignX = VBoxLayout::Left;
		vlayout->widthPolicy = VBoxLayout::Shrink;
		vlayout->setSpacing(10.f);

		for (const auto wand : manager.getWands()) {
			auto hlayout = std::make_unique<HBoxLayout>();
			hlayout->setSpacing(10.f);
			auto inventory = std::make_unique<SpellInventory>(wand->inventory);

			inventory->setOnModify([wand](viewable::Val<Shared<game::Spell>> spells) {
				wand->inventory.assign_range(spells);
			});

			auto image = std::make_unique<Image>(wand->getTexture());
			image->setKeepAspectRatio(true);
			auto height = inventory->getSize().y;
			image->setSize({height * 1.5f, height});
			hlayout->add(image | move);
			hlayout->add(inventory | move);
			vlayout->add(hlayout | move);
		}

		setLayout(vlayout | move);
	}
} // namespace flx::ui