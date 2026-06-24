#include "../../utils/Container/Map.h"
#include "SpellInventory.h"
#include "src/game/Spells/Spell.h"
#include "src/game/ui/NSpell.h"
#include "src/ui/render/NPainter.h"
#include "src/ui/widget/NWidget.h"
#include "src/utils/Assert.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <windows.h>

namespace flx::ui {
	void SpellInventory::updateSlotsGeometry() {
		const size_t count = slots.size();
		for (size_t i = 0; i < count; i++) {
			auto& [geometry, spell] = slots[i];
			geometry.position = vec2{i * (Spell::slotSize.x - Spell::outLine), 0}; // Local position
			geometry.size = Spell::slotSize;
			// slot.index = i;
		}

		frame.size = {
			count * (Spell::slotSize.x - Spell::outLine) + Spell::outLine,
			Spell::slotSize.y,
		};
	}

	std::pair<int, float> SpellInventory::getBestSlot(rect globalHitbox) const {
		int bestSlot = -1;
		float bestDistance = std::numeric_limits<float>::max();
		const vec2 globalCenter = globalHitbox.center();
		for (size_t i = 0; i < slots.size(); i++) {
			const auto& slot = slots[i];
			rect globalSlotGeometry = toGlobalBounds(slot.frame).offset(getPosition());
			if (globalSlotGeometry.overlaps(globalHitbox)) {
				const float disntance = (globalSlotGeometry.center() - globalCenter).lengthSquared();
				if (bestSlot == -1 || disntance < bestDistance) {
					bestSlot = static_cast<int>(i);
					bestDistance = disntance;
				}
			}
		}
		return {bestSlot, bestDistance};
	}

	void SpellInventory::updateHoveredSlot(vec2 mouseLocal) {
		hoveredSlot = -1;
		if (getFrame().contains(mouseLocal)) {
			const vec2 mouseLocal2 = mouseLocal - getPosition();
			for (const size_t i : slots.indices()) {
				if (slots[i].frame.contains(mouseLocal2)) {
					hoveredSlot = static_cast<int>(i);
					break;
				}
			}
		}
	}

	void SpellInventory::invokeOnModify() {
		if (!onModify)
			return;
		const auto spells = viewable::all(slots)
								.select([](const Slot& slot) -> Shared<game::Spell> {
									if (slot.spell)
										return slot.spell->spell;
									return {};
								});
		onModify(spells);
	}

	SpellInventory::SpellInventory(vec2 position, size_t slotCount) {
		setPosition(position);
		slots.resize(slotCount);
		updateSlotsGeometry();
		updateEnabled = true;
		typeID = makeTypeID<SpellInventory>();
	}

	void SpellInventory::draw(const UIPainter& canvas) const {
		sf::RectangleShape shape;
		shape.setOutlineColor({0, 0, 0});

		for (size_t i = 0; i < getCount(); i++) {
			shape.setPosition(slots[i].frame.position + vec2{Spell::outLine, Spell::outLine});
			shape.setOutlineThickness(Spell::outLine);
			const bool shouldHighlightSlot =
				(shouldHighlight && i == selectedSlot) || i == hoveredSlot;
			if (shouldHighlightSlot) {
				shape.setFillColor({200, 200, 200});
			} else {
				shape.setFillColor({140, 140, 140});
			}
			constexpr float width = Spell::slotSize.x - 2 * Spell::outLine;
			shape.setSize({width, width});
			canvas.draw(shape);
		}

		Widget::draw(canvas);
	}

	void SpellInventory::onDropQuery(const DropQuery& query, DropCollector& collector) {
		if (query.state.dragged->getTypeID() != makeTypeID<Spell>() ||
			!query.globalHitbox.overlaps(this->getGlobalBounds())) {
			selectedSlot = -1;
			return;
		}
		auto [bestSlot, bestDistance] = getBestSlot(query.globalHitbox);
		if (bestSlot != -1) {
			collector.candidates.emplace_back(this, -bestDistance);
			selectedSlot = bestSlot;
		}
		shouldHighlight = false;
	}

	void SpellInventory::onDropAccepted(const DropQuery& query, bool shouldDrop) {
		if (!shouldDrop) {
			shouldHighlight = true;
			return;
		}
		shouldHighlight = false;
		Widget* spellParent = query.state.dragged->getParent();
		SpellInventory* otherInventory = spellParent->convert<SpellInventory>();
		Spell* spell = query.state.dragged->convert<Spell>();
		if (!spell) {
			logger.error_and_throw("This should never happen...");
		}
		Unique<Object> spellObject;
		Unique<Object> replacedSpellObject;
		Spell* replacedSpell = getSpell(selectedSlot);

		if (spell == replacedSpell) {
			spell->isReleased = true;
			return;
		}

		const int oldIndex = spell->index;

		if (replacedSpell) {
			if (!otherInventory) {
				selectedSlot = -1;
				// TODO
				return;
			}
			updateSpellPosition(replacedSpell, otherInventory);
			replacedSpellObject = removeItem(replacedSpell);
		}

		updateSpellPosition(spell, this);

		if (otherInventory) {
			spellObject = otherInventory->removeItem(spell);
		} else {
			spellObject = spellParent->remove(spell);
		}

		if (otherInventory && replacedSpellObject) {
			otherInventory->addItem(std::move(replacedSpellObject), oldIndex);
		}

		addItem(std::move(spellObject), selectedSlot);

		selectedSlot = -1;
		shouldHighlight = false;

		invokeOnModify();
		if (otherInventory) {
			otherInventory->invokeOnModify();
		}
	}

	std::optional<NEventResult> SpellInventory::handleEvent(const NUIEvent& event) {
		if (event.windowEvent.rawEvent.is<sf::Event::MouseMoved>()) {
			updateHoveredSlot(event.localCtx.mouseLocal);
		}

		return Widget::handleEvent(event);
	}

	void SpellInventory::addItem(Unique<Object> spell, int index) {
		Spell* nspell = spell->convert<Spell>();
		if (!nspell) {
			logger.error_and_throw("Should only add NSpell item!");
		}
		if (slots.at(index).spell) {
			logger.error_and_throw("Slot index {} already has a spell", index);
		}
		nspell->index = index;
		nspell->isReleased = true;
		slots[index].spell = nspell;
		this->addToTop(std::move(spell));
	}

	Spell* SpellInventory::getSpell(int index) {
		if (!slots.valid(index)) {
			return {};
		}
		return slots[index].spell;
	}

	Unique<Object> SpellInventory::removeItem(Spell* spell) {
		slots.at(spell->index).spell = {};
		spell->index = -1;
		return this->remove(spell);
	}

	rect SpellInventory::getSlotGeometry(int index) const {
		return slots.at(index).frame;
	}

	void SpellInventory::setOnModify(OnModify onModify) {
		this->onModify = std::move(onModify);
	}

	// void NSpellInventory::onSpellReturn() {
	// 	shouldHighlight = false;
	// }

	void SpellInventory::updateSpellPosition(Spell* spell, const SpellInventory* to) {
		const vec2 globalPosition = spell->getGlobalPosition();
		spell->setPosition(globalPosition - to->getGlobalPosition());
	}
} // namespace flx::ui