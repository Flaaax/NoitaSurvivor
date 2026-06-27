#include "Spell.h"

#include "../../app/global/LocManager.h"
#include "SpellInventory.h"
#include "src/game/Spells/Spell.h"
#include "src/ui/context/Style.h"
#include "src/ui/context/WindowEvent.h"
#include "src/ui/elements/Image.h"
#include "src/ui/elements/RichText.h"
#include "src/ui/layout/BoxLayout.h"
#include "src/ui/layout/KeyValueLayout.h"
#include "src/ui/render/Painter.h"
#include "src/ui/widget/Widget.h"

#include <SFML/Graphics/Sprite.hpp>

namespace flx::ui {
	void Spell::updateTooltipSpec() {
		tooltipSpec.builder = tooltipBuilder;
		tooltipSpec.width = 400.f;
	}

	SUnique<Layout> Spell::tooltipBuilder(const Style& style, Object* self) {
		using flx::move;
		const auto nspell = static_cast<Spell*>(self);
		auto& loc = nspell->spell->getLoc();

		auto layout = std::make_unique<VBoxLayout>();
		layout->setPadding({10, 10, 10, 5});
		layout->setSpacing(8.f);

		auto titleLayout = std::make_unique<HBoxLayout>();
		auto image = std::make_unique<Image>(nspell->spell->getTexture());
		image->setSize(slotSize);
		image->setOutlineColor({0, 0, 0});
		image->setOutlineWidth(3.f);
		titleLayout->add(image | move);

		constexpr float lineSpacing = 0.85f;

		auto title = std::make_unique<RichText>(style.font, loc.title, 30u, lineSpacing);
		titleLayout->add(title | move);
		titleLayout->alignY = HBoxLayout::Center;
		titleLayout->setSpacing(10.f);

		layout->add(titleLayout | move);

		auto description = std::make_unique<RichText>(style.font, loc.description, 25u, lineSpacing);
		layout->add(description | move);
		auto layout1 = std::make_unique<KeyValueLayout>();
		layout1->setSpacing(1.f);

		const auto properties = nspell->spell->getDisplayedProperties();
		for (auto& [key, val] : properties) {
			auto keyText = std::make_unique<RichText>(style.font, key, 25u, lineSpacing);
			auto valueText = std::make_unique<RichText>(style.font, val, 25u, lineSpacing);
			layout1->add(keyText | move);
			layout1->add(valueText | move);
		}

		layout->add(layout1 | move);

		auto flavorLayout = std::make_unique<VBoxLayout>();
		flavorLayout->alignX = VBoxLayout::Center;
		flavorLayout->widthPolicy = VBoxLayout::Fill;
		flavorLayout->setPadding({0, 0, 0, 0});

		auto flavorText = std::make_unique<RichText>(style.font, loc.flavor, 23u, lineSpacing);
		flavorLayout->add(flavorText | move);
		layout->add(flavorLayout | move);

		return layout | move;
	}

	Spell::Spell(std::shared_ptr<game::Spell> spell, vec2 pos) : spell(std::move(spell)) {
		assertNotNull(this->spell.get());
		this->updateEnabled = true;
		frame.size = slotSize;
		frame.position = pos;
		this->typeID = makeTypeID<Spell>();
	}

	std::optional<EventResult> Spell::handleEvent(const UIEvent& event) {
		auto& raw = event.windowEvent.rawEvent;
		if (const auto e = raw.getIf<sf::Event::MouseButtonPressed>()) {
			if (e->button == sf::Mouse::Button::Left && this->frame.contains(event.localCtx.mouseLocal)) {
				isReleased = true;
				frame.setCenter(event.localCtx.mouseLocal);
				return EventResult{
					.handler = this,
					.result = EventResult::DragIntent{},
				};
			}
		} else if (raw.is<sf::Event::MouseMoved>() && this->frame.contains(event.localCtx.mouseLocal)) {
			updateTooltipSpec();
			return EventResult{
				.handler = this,
				.result = EventResult::HoverIntent{},
			};
		}
		// else if (!isDragged() && raw.is<sf::Event::MouseMoved>()) {
		// 	if (getHitbox().contains(event.localCtx.mouseLocal)) {
		// 		isHovered = true;
		// 	}
		// }
		return std::nullopt;
	}

	void Spell::update(float dt) {
		if (isDragged()) {
			t += dt;
			constexpr float p = 0.1f; // Quarter period.
			constexpr float A = 12.f; // Max rotation angle.
			while (t >= 4 * p)
				t -= 4 * p; // Loop the wave period.
			float tt = (3 * p <= t && t <= 4 * p) ? (t - 4 * p) : (p - std::abs(t - p));
			tt /= p; // Normalize to [-1, 1].
			const float sgn = std::abs(tt) < math::fepsilon ? 0.f : (tt / abs(tt));
			rotation = sgn * (1 - 1 / std::exp(std::abs(tt))) * A;
			return;
		}
		if (isReleased) {
			rotation = 0.f;
			const SpellInventory* inventory = getInventory();
			if (!inventory) {
				logger.error_and_throw("NSpell does not have an inventory, but released.");
			}
			const vec2 target = inventory->getSlotGeometry(index).position;
			const vec2 dir = target - frame.position;

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

	void Spell::draw(const UIPainter& canvas) const {
		if (!spell) {
			logger.error_and_throw("NSpell must be tied with a Spell!");
		}

		sf::Sprite sprite(spell->getTexture());
		const auto size = static_cast<vec2>(sprite.getTexture().getSize());
		const vec2 scale = slotSize / size;
		sprite.setRotation(sf::degrees(rotation));
		sprite.setOrigin(size / 2.f);
		sprite.setPosition(slotSize / 2.f);
		sprite.setScale(scale);

		canvas.draw(sprite);
	}

	void Spell::moveToSlot() {
		const vec2 target = getInventory()->getSlotGeometry(index).position;
		isReleased = false;
		setPosition(target);
	}

	const SpellInventory* Spell::getInventory() const {
		const auto parent = getParent();
		if (!parent) {
			return {};
		}
		return parent->convert<SpellInventory>();
	}
} // namespace flx::ui