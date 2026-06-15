#pragma once

#include "src/ui/widget/NWidget.h"

namespace flx::game {
	class Spell;
}

namespace flx::ui {
	class NSpellInventory;
	class NSpell;

	class NSpellInventory : public NWidget {
	private:
		struct Slot {
			rect frame;
			NSpell* spell{};
			// size_t index{};
		};

		int selectedSlot = -1;
		int hoveredSlot = -1;
		bool shouldHighlight{};
		flx::Vector<Slot> slots;

		using OnModify = std::function<void(flx::IValView<Shared<game::Spell>>)>;
		OnModify onModify{};

		void updateSlotsGeometry();
		std::pair<int, float> getBestSlot(rect globalHitbox) const;
		void updateHoveredSlot(vec2 mouseLocal);
		void invokeOnModify();

	public:
		NSpellInventory(vec2 position, size_t slotCount);

		void draw(const NUIPainter& canvas) const override;

		void onDropQuery(const NDropQuery& query, NDropCollector& collector) override;
		void onDropAccepted(const NDropQuery& query, bool shouldDrop) override;
		std::optional<NEventResult> handleEvent(const NUIEvent& event) override;

		void addItem(Unique<NObject> spell, int index);
		NSpell* getSpell(int index);
		Unique<NObject> removeItem(NSpell* spell);
		rect getSlotGeometry(int index) const;

		void setOnModify(OnModify onModify);

		// void onSpellReturn();

		size_t getCount() const {
			return slots.size();
		}

		static void updateSpellPosition(NSpell* spell, const NSpellInventory* to);
	};
} // namespace ui