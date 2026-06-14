#pragma once
#include "src/ui/NObject.h"
#include "src/utils/Pointer.h"


namespace flx::game {
	class Spell;
}

namespace flx::ui {
	class NSpellInventory;

	class NSpell : public ui::NObject {
	protected:
		friend class NSpellInventory;
		bool isReleased{};

		n_shared<game::Spell> spell;
		int index{};

		float rotation{};
		float t{};

		// bool isHovered{};

		void updateTooltipSpec();
		static n_unique<ui::NLayout> tooltipBuilder(const ui::NStyle& style, NObject* self);

	public:
		static constexpr vec2 slotSize = {45.f, 45.f};
		static constexpr float outLine = 3.f;

		explicit NSpell(std::shared_ptr<game::Spell> spell, vec2 pos = {});

		std::optional<ui::NEventResult> handleEvent(const ui::NUIEvent& event) override;
		void update(float dt) override;
		void draw(const ui::NUIPainter& canvas) const override;

		rect getHitbox() const override {
			return frame.getExpand({5.f, 5.f});
		}

		const NSpellInventory* getInventory() const;
	};
} // namespace flx::game