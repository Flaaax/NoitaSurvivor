#include "NSpell.h"

#include "../../app/global/AssetManager.h"
#include "../../app/global/LocManager.h"
#include "NSpellInventory.h"
#include "src/game/Spells/Spell.h"
#include "src/ui/context/NStyle.h"
#include "src/ui/elements/NImage.h"
#include "src/ui/elements/NRichText.h"
#include "src/ui/layout/NBoxLayout.h"
#include "src/ui/layout/NKeyValueLayout.h"
#include "src/ui/render/NCanvas.h"
#include "src/ui/widget/NWidget.h"

#include <SFML/Graphics/Sprite.hpp>

void NSpell::updateTooltipSpec() {
	tooltipSpec.builder = tooltipBuilder;
	tooltipSpec.width = 400.f;
}

n_unique<NLayout> NSpell::tooltipBuilder(const NStyle& style, NObject* self) {
	using Util::move;
	const NSpell* nspell = static_cast<NSpell*>(self);
	auto& loc = nspell->spell->getLoc();

	auto layout = std::make_unique<NVBoxLayout>();
	layout->setPadding({10, 10, 10, 5});
	layout->setSpacing(6.f);

	auto titleLayout = std::make_unique<NHBoxLayout>();
	auto image = std::make_unique<NImage>(nspell->spell->getTexture());
	image->setSize(slotSize);
	image->setOutlineColor({0, 0, 0});
	image->setOutlineWidth(3.f);
	titleLayout->add(image | move);

	auto title = std::make_unique<NRichText>(style.font, loc.title, 30u);
	titleLayout->add(title | move);
	titleLayout->alignY = NHBoxLayout::Center;
	titleLayout->setSpacing(10.f);

	layout->add(titleLayout | move);

	auto description = std::make_unique<NRichText>(style.font, loc.description, 25u);
	layout->add(description | move);
	auto layout1 = std::make_unique<NKeyValueLayout>();
	layout1->setSpacing(1.f);

	const auto properties = nspell->spell->getDisplayedProperties();
	for (auto& [key, val] : properties) {
		auto keyText = std::make_unique<NRichText>(style.font, key, 25u);
		auto valueText = std::make_unique<NRichText>(style.font, val, 25u);
		layout1->add(keyText | move);
		layout1->add(valueText | move);
	}

	layout->add(layout1 | move);

	auto flavorLayout = std::make_unique<NVBoxLayout>();
	flavorLayout->alignX = NVBoxLayout::Center;
	flavorLayout->widthPolicy = NVBoxLayout::Fill;
	flavorLayout->setPadding({0, 0, 0, 0});

		auto flavorText = std::make_unique<NRichText>(style.font, loc.flavor, 23u);
	flavorLayout->add(flavorText | move);
	layout->add(flavorLayout | move);

	return layout | move;
}

NSpell::NSpell(std::shared_ptr<Spell> spell, nvec2 pos) : spell(std::move(spell)) {
	assertNotNull(this->spell.get());
	this->updateEnabled = true;
	frame.size = slotSize;
	frame.position = pos;
	this->typeID = makeTypeID<NSpell>();
}

std::optional<NEventResult> NSpell::handleEvent(const NUIEvent& event) {
	auto& raw = event.windowEvent.rawEvent;
	if (const auto e = raw.getIf<sf::Event::MouseButtonPressed>()) {
		if (e->button == sf::Mouse::Button::Left && this->frame.contains(event.localCtx.mouseLocal)) {
			isReleased = true;
			frame.setCenter(event.localCtx.mouseLocal);
			return NEventResult{
				.handler = this,
				.result = NEventResult::DragIntent{},
			};
		}
	} else if (raw.is<sf::Event::MouseMoved>() && this->frame.contains(event.localCtx.mouseLocal)) {
		updateTooltipSpec();
		return NEventResult{
			.handler = this,
			.result = NEventResult::HoverIntent{},
		};
	}
	// else if (!isDragged() && raw.is<sf::Event::MouseMoved>()) {
	// 	if (getHitbox().contains(event.localCtx.mouseLocal)) {
	// 		isHovered = true;
	// 	}
	// }
	return std::nullopt;
}

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
		return;
	}
	if (isReleased) {
		rotation = 0.f;
		const NSpellInventory* inventory = getInventory();
		if (!inventory) {
			LoggerOld::error_and_throw("NSpell does not have an inventory, but released.");
		}
		const nvec2 target = inventory->getSlotGeometry(index).position;
		const nvec2 dir = target - frame.position;

		const float len = dir.lengthSquared();

		// Snap to the slot when it is close enough.
		if (len < 0.5f) {
			isReleased = false;
			frame.position = target;
			return;
		}

		// Smoothly move toward the target slot with exponential decay.
		constexpr float DECAY_FACTOR = 10.0f; // Larger values snap faster.
		const float moveRatio = 1.0f - std::exp(-DECAY_FACTOR * dt);
		frame.position += dir * moveRatio;
	}
}

void NSpell::draw(const NCanvas& canvas) const {
	if (!spell) {
		LoggerOld::error_and_throw("NSpell must be tied with a Spell!");
	}

	sf::Sprite sprite(spell->getTexture());
	const auto size = static_cast<nvec2>(sprite.getTexture().getSize());
	const nvec2 scale = slotSize / size;
	sprite.setRotation(sf::degrees(rotation));
	sprite.setOrigin(size / 2.f);
	sprite.setPosition(slotSize / 2.f);
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