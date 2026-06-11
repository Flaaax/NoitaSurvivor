#pragma once

#include "src/ui/widget/NWidget.h"

class Spell;
class NSpellInventory;
class NSpell;

class NSpellInventory : public NWidget {
private:
	struct Slot {
		nrect frame;
		NSpell* spell{};
		// size_t index{};
	};

	int selectedSlot = -1;
	int hoveredSlot = -1;
	bool shouldHighlight{};
	Util::Vector<Slot> slots;

	using OnModify = std::function<void(Util::ValEnumerableView<n_shared<Spell>>)>;
	OnModify onModify{};

	void updateSlotsGeometry();
	std::pair<int, float> getBestSlot(nrect globalHitbox) const;
	void updateHoveredSlot(nvec2 mouseLocal);
	void invokeOnModify();

public:
	NSpellInventory(nvec2 position, size_t slotCount);

	void draw(const NUIPainter& canvas) const override;

	void onDropQuery(const NDropQuery& query, NDropCollector& collector) override;
	void onDropAccepted(const NDropQuery& query, bool shouldDrop) override;
	std::optional<NEventResult> handleEvent(const NUIEvent& event) override;

	void addItem(n_unique<NObject> spell, int index);
	NSpell* getSpell(int index);
	n_unique<NObject> removeItem(NSpell* spell);
	nrect getSlotGeometry(int index) const;

	void setOnModify(OnModify onModify);

	// void onSpellReturn();

	size_t getCount() const {
		return slots.size();
	}

	static void updateSpellPosition(NSpell* spell, const NSpellInventory* to);
};