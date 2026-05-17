#pragma once

#include "../NObject.h"
#include "src/ui/widget/NWidget.h"

class Spell;
class NSpellInventory;
class NSpell;

// class NSpellInventoryOld : public NObject {
// private:
// 	friend class NSpell;
// 	std::vector<NSpellSlot> slots;
// 	bool modified = true;
// 	std::function<void(const NSpellInventory&)> onModify;
//
// 	// Based on self position and count of the inventory
// 	nvec2 calcSlotPosition(size_t index) const;
// 	void updateSlots();
//
// public:
// 	// The inventory should be empty, but has proper size
// 	NSpellInventoryOld(size_t count, const nvec2& pos);
//
// 	void setOnModify(auto&& callback) {
// 		onModify = std::forward<decltype(callback)>(callback);
// 	}
//
// 	void setCount(size_t count);
//
// 	size_t getCount() const {
// 		return slots.size();
// 	}
//
// 	void draw(const NCanvas& canvas) const override;
// 	void bindSpell(NSpell* spell, size_t index);
// 	void addSpell(NSpell* spell, size_t index);
// 	void addSpell(n_shared<Spell> spell, size_t index);
// 	NSpell* removeSpell(size_t index);
// 	void destroyAllSpell();
// 	void update(float dt) override;
//
// 	bool isModified() const {
// 		return modified;
// 	}
//
// 	n_shared<Spell> getSpell(size_t index) const;
// 	void setFrom(const std::vector<n_shared<Spell>>& spells);
// };

class NSpellInventory : public NWidget {
private:
	struct Slot {
		nrect geometry;
		NSpell* spell{};
		// size_t index{};
	};

	int selectedSlot{};
	bool shouldHighlight{};
	Util::Vector<Slot> slots;
	void updateSlotsGeometry();
	std::pair<int, float> getBestSlot(nrect globalGeometry) const;

public:
	NSpellInventory(nvec2 position, size_t slotCount);

	void draw(const NCanvas& canvas) const override;

	void onDropQuery(const NDropQuery& query, NDropCollector& collector) override;
	void onDropAccepted(const NDropQuery& query, bool shouldDrop) override;

	void addItem(n_unique<NObject> spell, int index);
	NSpell* getSpell(int index);
	n_unique<NObject> removeItem(NSpell* spell);
	nrect getSlotGeometry(int index)const;

	size_t getCount() const {
		return slots.size();
	}
};