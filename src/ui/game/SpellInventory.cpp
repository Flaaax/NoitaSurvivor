#include "SpellInventory.h"
#include "../../utils/Container/Container.h"
#include "../Renderer.h"
#include "../widget/NWidget.h"
#include "src/game/Spells/Spell.h"
#include "src/utils/Assert.h"

nvec2 NSpellInventory::calcSlotPosition(size_t index) const {
	return getPosition() + nvec2{index * (NSpell::slotSize - NSpell::outLine), 0};
}

void NSpellInventory::updateSlots() {
	size_t count = getCount();
	for (size_t i = 0; i < count; i++) {
		auto& slot = slots[i];
		slot.geometry.size = {NSpell::slotSize, NSpell::slotSize};
		slot.geometry.position = calcSlotPosition(i);
		slot.index = i;
		slot.inventory = this;
	}

	m_geometry.size = {count * (NSpell::slotSize - NSpell::outLine) + NSpell::outLine,
					   NSpell::slotSize};
}

NSpellInventory::NSpellInventory(size_t count, const nvec2& pos) {
	setPosition(pos);
	slots.resize(count);
	updateSlots();
	setUpdate(true);
	ID = Util::TypeName<NSpellInventory>();
}

void NSpellInventory::setCount(size_t count) {
	slots.resize(count);
	updateSlots();
	modified = true;
}

void NSpellInventory::draw(Renderer& renderer) const {
	for (size_t i = 0; i < getCount(); i++) {
		sf::RectangleShape shape;
		shape.setPosition(calcSlotPosition(i) + nvec2{NSpell::outLine, NSpell::outLine});
		shape.setOutlineThickness(NSpell::outLine);
		shape.setFillColor({140, 140, 140});
		shape.setOutlineColor({0, 0, 0});
		auto width = NSpell::slotSize - 2 * NSpell::outLine;
		shape.setSize({width, width});
		renderer.drawGui(shape);
	}
}

void NSpellInventory::bindSpell(NSpell* spell, size_t index) {
	assertNotNull(spell);
	assertNotNull(spell->spell.get());
	assertNotNull(getParent());
	assertWithMsg(!slots[index].spell, "Spell already exists!");
	assertValidIndex(index, getCount());
	getParent()->addAfter(this, spell);
	slots[index].spell = spell;
	spell->slot = &slots[index];
	modified = true;
}

void NSpellInventory::addSpell(NSpell* spell, size_t index) {
	bindSpell(spell, index);
	spell->setPosition(spell->slot->geometry.position);
}

void NSpellInventory::addSpell(n_shared<Spell> spell, size_t index) {
	addSpell(new NSpell(std::move(spell)), index);
}

NSpell* NSpellInventory::removeSpell(size_t index) {
	assertValidIndex(index, getCount());
	NSpell* ret{};
	std::swap(ret, slots[index].spell);
	if (ret) {
		ret->slot = nullptr;
		modified = true;
	}
	return ret;
}

void NSpellInventory::destroyAllSpell() {
	for (auto& slot : slots) {
		if (slot.spell) {
			getParent()->destroy(slot.spell);
			slot.spell = {};
			modified = true;
		}
	}
}

void NSpellInventory::update(float dt) {
	if (modified) {
		modified = false;
		if (onModify)
			onModify(*this);
	}
}

n_shared<Spell> NSpellInventory::getSpell(size_t index) const {
	assertValidIndex(index, getCount());
	auto& slot = slots[index];
	return slot.spell ? slot.spell->spell : nullptr;
}

void NSpellInventory::setFrom(const std::vector<n_shared<Spell>>& spells) {
	destroyAllSpell();
	setCount(spells.size());
	for (size_t i = 0; i < getCount(); i++) {
		if (spells[i]) {
			addSpell(spells[i], i);
		}
	}
}

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

NSpell::NSpell(std::shared_ptr<Spell> spell, const nvec2& pos) : spell(std::move(spell)) {
	assertNotNull(this->spell.get());

	setUpdate(true);
	stat = None;
	m_geometry.size = {slotSize, slotSize};
	m_geometry.position = pos;
}

bool NSpell::handleEvent(const sf::Event& event) {
	if (event.is<sf::Event::MouseButtonPressed>()) {
		if (m_geometry.contains(NWindow::mouseRenderPos)) {
			stat = Dragged;
			getParent()->moveToTop(this); // must have a parent
			t = 0.f;
			return true;
		}
	} else if (event.is<sf::Event::MouseButtonReleased>()) {
		if (stat != Dragged) {
			return false;
		}
		rotation = 0.f;
		stat = Released;
		Util::Vector<NSpellSlot*> slots;

		for (const auto obj : getParent()->getObjects()) {
			if (obj->ID == Util::TypeName<NSpellInventory>()) {
				for (const auto inventory = static_cast<NSpellInventory*>(obj); auto& slot : inventory->slots) {
					if (slot.geometry.overlaps(this->m_geometry)) {
						slots += &slot;
					}
				}
			}
		}

		// no target slot
		if (slots.size() == 0)
			return true;

		// find nearest slot
		float minLength = std::numeric_limits<float>::max();
		NSpellSlot* other_slot = slots.front();
		for (auto myslot : slots) {
			auto len = (myslot->geometry.position - m_geometry.position).lengthSquared();
			if (len < minLength) {
				other_slot = myslot;
				minLength = len;
			}
		}
		assertNotNull(other_slot);

		// Need not change slots
		if (other_slot == slot)
			return true;

		NSpellSlot* original_slot = this->slot;
		NSpell* other_spell = other_slot->spell;

		original_slot->inventory->removeSpell(original_slot->index);
		other_slot->inventory->removeSpell(other_slot->index);

		// bind the other spell to this slot
		if (other_spell) {
			// Logger::error("Other slot have spell: {}", other_spell->spell->texture);
			original_slot->inventory->bindSpell(other_spell, original_slot->index);
			other_spell->stat = Released;
		}

		// bind this to other slot
		other_slot->inventory->bindSpell(this, other_slot->index);

		return true;
	}

	return false;
}

void NSpell::update(float dt) {
	if (stat == Released) {
		auto dir = slot->geometry.position - m_geometry.position;
		auto len = dir.lengthSquared();

		// Snap to the slot when it is close enough.
		if (len < 0.5f) {
			stat = None;
			m_geometry.position = slot->geometry.position;
			return;
		}

		// Smoothly move toward the target slot with exponential decay.
		constexpr float DECAY_FACTOR = 10.0f; // Larger values snap faster.
		float moveRatio = 1.0f - std::exp(-DECAY_FACTOR * dt);
		m_geometry.position += dir * moveRatio;
	} else if (stat == Dragged) {
		m_geometry.setCenter(NWindow::mouseRenderPos);

		t += dt;
		constexpr float p = 0.1f; // Quarter period.
		constexpr float A = 12.f; // Max rotation angle.
		while (t >= 4 * p)
			t -= 4 * p; // Loop the wave period.
		float tt = (3 * p <= t && t <= 4 * p) ? (t - 4 * p) : (p - std::abs(t - p));
		tt /= p; // Normalize to [-1, 1].
		const float sgn = std::abs(tt) < nmath::n_epsilon ? 0.f : (tt / abs(tt));
		rotation = sgn * (1 - 1 / std::exp(std::abs(tt))) * A;
	}
}

void NSpell::draw(Renderer& renderer) const {
	if (!spell)
		return;
	sf::Sprite sprite(spell->getTexture());
	const nvec2 size = sprite.getTexture().getSize();
	auto scale = NSpell::slotSize / size.x;
	sprite.setRotation(sf::degrees(rotation));
	sprite.setOrigin(size / 2.f);
	sprite.setPosition(m_geometry.position + scale * size / 2.f);
	sprite.setScale({scale, scale});

	renderer.drawGui(sprite);
}