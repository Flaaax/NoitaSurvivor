#include "NSpell.h"

#include "NSpellInventory.h"
#include "src/game/Spells/Spell.h"
#include "src/ui/widget/NWidget.h"

NSpell::NSpell(std::shared_ptr<Spell> spell, nvec2 pos) : spell(std::move(spell)) {
	assertNotNull(this->spell.get());
	this->updateEnabled = true;
	geometry.size = slotSize;
	geometry.position = pos;
	this->typeID = makeTypeID<NSpell>();
}

std::optional<NEventResult> NSpell::handleEvent(const NUIEvent& event) {
	auto& raw = event.ctx.rawEvent;
	if (const auto e = raw.getIf<sf::Event::MouseButtonPressed>()) {
		if (e->button == sf::Mouse::Button::Left && this->getHitbox().contains(event.localCtx.mouseLocal)) {
			isReleased = false;
			return NEventResult{
				.handler = this,
				.result = NEventResult::Dragged{},
			};
		}
	}
	return std::nullopt;
}

// bool NSpell::handleEventOld(const sf::Event& event) {
// 	if (event.is<sf::Event::MouseButtonPressed>()) {
// 		if (geometry.contains(NWindow::mouseRenderPos)) {
// 			stat = Dragged;
// 			getParent()->moveToTop(this); // must have a parent
// 			t = 0.f;
// 			return true;
// 		}
// 	} else if (event.is<sf::Event::MouseButtonReleased>()) {
// 		if (stat != Dragged) {
// 			return false;
// 		}
// 		rotation = 0.f;
// 		stat = Released;
// 		Util::Vector<NSpellSlot*> slots;
//
// 		for (const auto obj : getParent()->getObjects()) {
// 			if (obj->ID == Util::typeName<NSpellInventory>()) {
// 				for (const auto inventory = static_cast<NSpellInventory*>(obj); auto& slot : inventory->slots) {
// 					if (slot.geometry.overlaps(this->geometry)) {
// 						slots += &slot;
// 					}
// 				}
// 			}
// 		}
//
// 		// no target slot
// 		if (slots.size() == 0)
// 			return true;
//
// 		// find nearest slot
// 		float minLength = std::numeric_limits<float>::max();
// 		NSpellSlot* other_slot = slots.front();
// 		for (auto myslot : slots) {
// 			auto len = (myslot->geometry.position - geometry.position).lengthSquared();
// 			if (len < minLength) {
// 				other_slot = myslot;
// 				minLength = len;
// 			}
// 		}
// 		assertNotNull(other_slot);
//
// 		// Need not change slots
// 		if (other_slot == slot)
// 			return true;
//
// 		NSpellSlot* original_slot = this->slot;
// 		NSpell* other_spell = other_slot->spell;
//
// 		original_slot->inventory->removeSpell(original_slot->index);
// 		other_slot->inventory->removeSpell(other_slot->index);
//
// 		// bind the other spell to this slot
// 		if (other_spell) {
// 			// Logger::error("Other slot have spell: {}", other_spell->spell->texture);
// 			original_slot->inventory->bindSpell(other_spell, original_slot->index);
// 			other_spell->stat = Released;
// 		}
//
// 		// bind this to other slot
// 		other_slot->inventory->bindSpell(this, other_slot->index);
//
// 		return true;
// 	}
//
// 	return false;
// }

void NSpell::update(float dt) {
	if (isDragged()) {
		t += dt;
		constexpr float p = 0.1f; // Quarter period.
		constexpr float A = 12.f; // Max rotation angle.
		while (t >= 4 * p)
			t -= 4 * p; // Loop the wave period.
		float tt = (3 * p <= t && t <= 4 * p) ? (t - 4 * p) : (p - std::abs(t - p));
		tt /= p; // Normalize to [-1, 1].
		const float sgn = std::abs(tt) < nmath::n_epsilon ? 0.f : (tt / abs(tt));
		rotation = sgn * (1 - 1 / std::exp(std::abs(tt))) * A;
	} else if (isReleased) {
		const nrect slotGeometry = getInventory()->getSlotGeometry(index);
		const nvec2 dir = slotGeometry.position - geometry.position;
		const float len = dir.lengthSquared();

		// Snap to the slot when it is close enough.
		if (len < 0.5f) {
			isReleased = false;
			geometry.position = slotGeometry.position;
			return;
		}

		// Smoothly move toward the target slot with exponential decay.
		constexpr float DECAY_FACTOR = 10.0f; // Larger values snap faster.
		const float moveRatio = 1.0f - std::exp(-DECAY_FACTOR * dt);
		geometry.position += dir * moveRatio;
	}
}

void NSpell::draw(const NCanvas& canvas) const {
	if (!spell) {
		Logger::error_and_throw("NSpell must be tied with a Spell!");
	}

	sf::Sprite sprite(spell->getTexture());
	const nvec2 size = sprite.getTexture().getSize();
	auto scale = slotSize / size.x;
	sprite.setRotation(sf::degrees(rotation));
	sprite.setOrigin(size / 2.f);
	sprite.setPosition(geometry.position + scale * size / 2.f);
	sprite.setScale(scale);

	canvas.draw(sprite);
}

NSpellInventory* NSpell::getInventory() const {
	NWidget* parent = getParent();
	if (!parent) {
		return {};
	}
	return parent->convert<NSpellInventory>();
}