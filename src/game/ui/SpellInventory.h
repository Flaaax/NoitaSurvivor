#pragma once

#include "src/ui/widget/NWidget.h"
#include "src/utils/Container/Viewable.h"

namespace flx::game {
	class Spell;
}

namespace flx::ui {
	class SpellInventory;
	class Spell;

	class SpellInventory : public Widget {
	private:
		struct Slot {
			rect frame;
			Spell* spell{};
			// size_t index{};
		};

		int selectedSlot = -1;
		int hoveredSlot = -1;
		bool shouldHighlight{};
		Vector<Slot> slots;

		using OnModify = std::function<void(viewable::Val<Shared<game::Spell>>)>;
		OnModify onModify{};

		void updateSlotsGeometry();
		Pair<int, float> getBestSlot(rect globalHitbox) const;
		void updateHoveredSlot(vec2 mouseLocal);
		void invokeOnModify();

	public:
		SpellInventory(vec2 position, size_t slotCount);

		void draw(const UIPainter& canvas) const override;

		void onDropQuery(const DropQuery& query, DropCollector& collector) override;
		void onDropAccepted(const DropQuery& query, bool shouldDrop) override;
		std::optional<NEventResult> handleEvent(const NUIEvent& event) override;

		void addItem(Unique<Object> spell, int index);
		Spell* getSpell(int index);
		Unique<Object> removeItem(Spell* spell);
		rect getSlotGeometry(int index) const;

		void setOnModify(OnModify onModify);

		// void onSpellReturn();

		u64 getCount() const {
			return slots.size();
		}

		static void updateSpellPosition(Spell* spell, const SpellInventory* to);
	};
} // namespace flx::ui