#pragma once

#include "src/ui/widget/Widget.h"
#include "src/utils/Container/Viewable.h"

namespace flx::game {
	class Wand;
	class Spell;
} // namespace flx::game

namespace flx::ui {
	class SpellInventory;
	class Spell;

	class SpellInventory : public Widget {
	private:
		struct Slot {
			rect frame;
			Ref<Spell> spell{};
			// size_t index{};
		};

		using InventoryView = viewable::Val<Shared<game::Spell>>;

		int selectedSlot = -1;
		int hoveredSlot = -1;
		bool shouldHighlight{};
		Vector<Slot> slots;

		using OnModify = std::function<void(InventoryView)>;
		OnModify onModify{};

		void updateSlotsGeometry();
		Pair<int, float> getBestSlot(rect globalHitbox) const;
		void updateHoveredSlot(vec2 mouseLocal);
		void invokeOnModify();

	public:
		SpellInventory(vec2 position, u64 slotCount);
		explicit SpellInventory(Span<Shared<game::Spell>> spells);

		void draw(const UIPainter& canvas) const override;

		void onDropQuery(const DropQuery& query, DropCollector& collector) override;
		void onDropAccepted(const DropQuery& query, bool shouldDrop) override;
		std::optional<EventResult> handleEvent(const UIEvent& event) override;

		void setItems(Span<Shared<game::Spell>> spells);
		void addItem(SUnique<Object> spell, int index);
		Ref<Spell> getSpell(int index);
		SUnique<Object> removeItem(Ref<Spell> spell);
		rect getSlotGeometry(int index) const;

		void setOnModify(OnModify onModify);

		// void onSpellReturn();

		u64 getCount() const {
			return slots.size();
		}

		static void updateSpellPosition(Ref<Spell> spell, CRef<SpellInventory> to);
	};
} // namespace flx::ui