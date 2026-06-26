#pragma once
#include "src/ui/Object.h"
#include "src/utils/Pointer.h"


namespace flx::game {
	class Spell;
}

namespace flx::ui {
	class SpellInventory;

	class Spell : public Object {
	protected:
		friend class SpellInventory;
		bool isReleased{};

		Shared<game::Spell> spell;
		int index{};

		float rotation{};
		float t{};

		// bool isHovered{};

		void updateTooltipSpec();
		static Unique<ui::Layout> tooltipBuilder(const ui::Style& style, Object* self);

	public:
		static constexpr vec2 slotSize = {45.f, 45.f};
		static constexpr float outLine = 3.f;

		explicit Spell(Shared<game::Spell> spell, vec2 pos = {});

		std::optional<ui::EventResult> handleEvent(const ui::UIEvent& event) override;
		void update(float dt) override;
		void draw(const ui::UIPainter& canvas) const override;

		rect getHitbox() const override {
			return frame.getExpand({5.f, 5.f});
		}

		const SpellInventory* getInventory() const;
	};
} // namespace flx::game