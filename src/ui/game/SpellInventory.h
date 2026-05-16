#pragma once

#include"../NObject.h"

class Spell;
class NSpellInventory;
class NSpell;

struct NSpellSlot {
	nrect geometry;
	NSpell* spell{};
	size_t index{};
	NSpellInventory* inventory{};
};

class NSpellInventoryOld :public NObject {
private:
	friend class NSpell;
	std::vector<NSpellSlot> slots;
	bool modified = true;
	std::function<void(const NSpellInventory&)> onModify;

	//Based on self position and count of the inventory
	nvec2 calcSlotPosition(size_t index)const;
	void updateSlots();
public:
	//The inventory should be empty, but has proper size
	NSpellInventoryOld(size_t count, const nvec2& pos);

	void setOnModify(auto&& callback) { onModify = std::forward<decltype(callback)>(callback); }
	void setCount(size_t count);
	size_t getCount()const { return slots.size(); }
	void draw(Renderer& renderer)const override;
	void bindSpell(NSpell* spell, size_t index);
	void addSpell(NSpell* spell, size_t index);
	void addSpell(n_shared<Spell> spell, size_t index);
	NSpell* removeSpell(size_t index);
	void destroyAllSpell();
	void update(float dt) override;
	bool isModified()const { return modified; }
	n_shared<Spell> getSpell(size_t index)const;
	void setFrom(const std::vector<n_shared<Spell>>& spells);
};
