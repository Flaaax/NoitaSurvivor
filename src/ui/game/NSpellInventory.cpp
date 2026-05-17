#include "NSpellInventory.h"
#include "../../utils/Container/Container.h"
#include "../widget/NWidget.h"
#include "src/game/Spells/Spell.h"
#include "src/ui/game/NSpell.h"
#include "src/utils/Assert.h"

#include <windows.h>

// nvec2 NSpellInventoryOld::calcSlotPosition(size_t index) const {
// 	return getPosition() + nvec2{index * (NSpell::slotSize - NSpell::outLine), 0};
// }
//
// void NSpellInventoryOld::updateSlots() {
// 	size_t count = getCount();
// 	for (size_t i = 0; i < count; i++) {
// 		auto& slot = slots[i];
// 		slot.geometry.size = {NSpell::slotSize, NSpell::slotSize};
// 		slot.geometry.position = calcSlotPosition(i);
// 		slot.index = i;
// 		slot.inventory = this;
// 	}
//
// 	geometry.size = {count * (NSpell::slotSize - NSpell::outLine) + NSpell::outLine,
// 					 NSpell::slotSize};
// }
//
// NSpellInventoryOld::NSpellInventory(size_t count, const nvec2& pos) {
// 	setPosition(pos);
// 	slots.resize(count);
// 	updateSlots();
// 	setUpdate(true);
// 	ID = Util::TypeName<NSpellInventory>();
// }
//
// void NSpellInventoryOld::setCount(size_t count) {
// 	slots.resize(count);
// 	updateSlots();
// 	modified = true;
// }
//
// void NSpellInventoryOld::draw(const NCanvas& canvas) const {
// 	for (size_t i = 0; i < getCount(); i++) {
// 		sf::RectangleShape shape;
// 		shape.setPosition(calcSlotPosition(i) + nvec2{NSpell::outLine, NSpell::outLine});
// 		shape.setOutlineThickness(NSpell::outLine);
// 		shape.setFillColor({140, 140, 140});
// 		shape.setOutlineColor({0, 0, 0});
// 		auto width = NSpell::slotSize - 2 * NSpell::outLine;
// 		shape.setSize({width, width});
// 		canvas.drawUI(shape);
// 	}
// }
//
// void NSpellInventoryOld::bindSpell(NSpell* spell, size_t index) {
// 	assertNotNull(spell);
// 	assertNotNull(spell->spell.get());
// 	assertNotNull(getParent());
// 	assertWithMsg(!slots[index].spell, "Spell already exists!");
// 	assertValidIndex(index, getCount());
// 	getParent()->addAfter(this, spell);
// 	slots[index].spell = spell;
// 	spell->slot = &slots[index];
// 	modified = true;
// }
//
// void NSpellInventoryOld::addSpell(NSpell* spell, size_t index) {
// 	bindSpell(spell, index);
// 	spell->setPosition(spell->slot->geometry.position);
// }
//
// void NSpellInventoryOld::addSpell(n_shared<Spell> spell, size_t index) {
// 	addSpell(new NSpell(std::move(spell)), index);
// }
//
// NSpell* NSpellInventoryOld::removeSpell(size_t index) {
// 	assertValidIndex(index, getCount());
// 	NSpell* ret{};
// 	std::swap(ret, slots[index].spell);
// 	if (ret) {
// 		ret->slot = nullptr;
// 		modified = true;
// 	}
// 	return ret;
// }
//
// void NSpellInventoryOld::destroyAllSpell() {
// 	for (auto& slot : slots) {
// 		if (slot.spell) {
// 			getParent()->destroy(slot.spell);
// 			slot.spell = {};
// 			modified = true;
// 		}
// 	}
// }
//
// void NSpellInventoryOld::update(float dt) {
// 	if (modified) {
// 		modified = false;
// 		if (onModify)
// 			onModify(*this);
// 	}
// }
//
// n_shared<Spell> NSpellInventoryOld::getSpell(size_t index) const {
// 	assertValidIndex(index, getCount());
// 	auto& slot = slots[index];
// 	return slot.spell ? slot.spell->spell : nullptr;
// }
//
// void NSpellInventoryOld::setFrom(const std::vector<n_shared<Spell>>& spells) {
// 	destroyAllSpell();
// 	setCount(spells.size());
// 	for (size_t i = 0; i < getCount(); i++) {
// 		if (spells[i]) {
// 			addSpell(spells[i], i);
// 		}
// 	}
// }

// NSpellInventory* NSpellInventory::create(NWidget* widget, Inventory& inventory, const nvec2& pos) {
//	Inventory copy;
//	inventory.swap(copy);
//	inventory.resize(copy.size());
//	auto ret = new NSpellInventory(inventory, pos);
//	widget->add(ret);
//	for (size_t i = 0; i < copy.size(); i++) {
//		if (!copy[i])continue;
//		auto nspell = new NSpell(std::move(copy[i]));
//		ret->addSpell(nspell, i);
//	}
//	return ret;
// }

void NSpellInventory::updateSlotsGeometry() {
	const size_t count = slots.size();
	for (size_t i = 0; i < count; i++) {
		auto& [geometry, spell] = slots[i];
		geometry.position = nvec2{i * (NSpell::slotSize.x - NSpell::outLine), 0}; // Local position
		geometry.size = NSpell::slotSize;
		// slot.index = i;
	}

	geometry.size = {
		count * (NSpell::slotSize.x - NSpell::outLine) + NSpell::outLine,
		NSpell::slotSize.y,
	};
}

std::pair<int, float> NSpellInventory::getBestSlot(nrect globalGeometry) const {
	int bestSlot = static_cast<int>(slots.size());
	float bestDistance = std::numeric_limits<float>::max();
	const nvec2 globalCenter = globalGeometry.center();
	for (size_t i = 0; i < slots.size(); i++) {
		if (const auto& slot = slots[i]; getGlobalGeometry(slot.geometry).overlaps(globalGeometry)) {
			const float disntance = (slot.geometry.center() - globalCenter).lengthSquared();
			if (bestSlot == -1 || disntance < bestDistance) {
				bestSlot = static_cast<int>(i);
				bestDistance = disntance;
			}
		}
	}
	return {bestSlot, bestDistance};
}

NSpellInventory::NSpellInventory(nvec2 position, size_t slotCount) {
	setPosition(position);
	slots.resize(slotCount);
	updateSlotsGeometry();
	updateEnabled = true;
	typeID = Util::typeName<NSpellInventory>();
}

// std::optional<NEventResult> NSpellInventory::handleEvent(const NEvent& event) {
// 	if (const auto result = NWidget::handleEvent(event)) {
// 		return result;
// 	}
//
// 	return std::nullopt;
// }

void NSpellInventory::draw(const NCanvas& canvas) const {
	sf::RectangleShape shape;
	shape.setOutlineColor({0, 0, 0});

	for (size_t i = 0; i < getCount(); i++) {
		shape.setPosition(slots[i].geometry.position + nvec2{NSpell::outLine, NSpell::outLine});
		shape.setOutlineThickness(NSpell::outLine);
		if (i != selectedSlot && shouldHighlight) {
			shape.setFillColor({140, 140, 140});
		} else {
			shape.setFillColor({200, 200, 200});
		}
		constexpr float width = NSpell::slotSize.x - 2 * NSpell::outLine;
		shape.setSize({width, width});
		canvas.draw(shape);
	}

	NWidget::draw(canvas);
}

void NSpellInventory::onDropQuery(const NDropQuery& query, NDropCollector& collector) {
	if (query.state.dragged->getTypeID() != makeTypeID<NSpell>() ||
		!query.globalHitbox.overlaps(this->getGlobalGeometry())) {
		return;
	}
	auto [bestSlot, bestDistance] = getBestSlot(query.globalHitbox);
	if (bestSlot != -1) {
		collector.candidates.emplace_back(this, -bestDistance);
		selectedSlot = bestSlot;
	} else {
		selectedSlot = -1;
	}
	shouldHighlight = false;
}

void NSpellInventory::onDropAccepted(const NDropQuery& query, bool shouldDrop) {
	if (!shouldDrop) {
		shouldHighlight = true;
		return;
	}
	shouldHighlight = false;
	NWidget* spellParent = query.state.dragged->getParent();
	NSpellInventory* otherInventory = spellParent->convert<NSpellInventory>();
	NSpell* spell = query.state.dragged->convert<NSpell>();
	if (!spell) {
		Logger::error_and_throw("This should never happen...");
	}
	n_unique<NObject> spellObject;
	NSpell* originalSpell = getSpell(selectedSlot);

	if (originalSpell) {
		if (!otherInventory) {
			selectedSlot = -1;
			// TODO
			return;
		}
		n_unique<NObject> originalSpellObject = removeItem(originalSpell);
		otherInventory->addItem(std::move(originalSpellObject), spell->index);
	}

	if (otherInventory) {
		spellObject = otherInventory->removeItem(spell);
	} else {
		spellObject = spellParent->remove(spell);
	}

	addItem(std::move(spellObject), selectedSlot);

	selectedSlot = -1;
}

void NSpellInventory::addItem(n_unique<NObject> spell, int index) {
	NSpell* nspell = spell->convert<NSpell>();
	if (!nspell) {
		Logger::error_and_throw("Should only add NSpell item!");
	}
	if (slots.at(index).spell) {
		Logger::error_and_throw("Slot index {} already has a spell", index);
	}
	nspell->index = index;
	nspell->isReleased = true;
	slots[index].spell = nspell;
	this->addToTop(std::move(spell));
}

NSpell* NSpellInventory::getSpell(int index) {
	if (!slots.valid(index)) {
		return {};
	}
	return slots[index].spell;
}

n_unique<NObject> NSpellInventory::removeItem(NSpell* spell) {
	spell->index = -1;
	return this->remove(spell);
}

nrect NSpellInventory::getSlotGeometry(int index) const {
	return slots.at(index).geometry;
}