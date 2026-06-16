// #include"src/game/Spells/Modifiers/AddExplosionSpell.h"
#include "Wand.h"
#include "../../app/global/AssetManager.h"
#include "src/app/global/Loader.h"
#include "src/game/Spells/Modifiers/BasicModifiers.h"
#include "src/game/Spells/Projectiles/BasicProjectiles.h"
#include "src/game/Spells/SpellBlock.h"
#include "src/ui/render/NPainter.h"
#include "src/utils/Logging/Logger.h"
#include "src/utils/Random.h"
#include "src/utils/Text/Format.h"

#include <iomanip>
#include <sstream>

namespace flx::game {
	constexpr std::string_view defaultTexture = "gfx/wands/noita/wand_0000.png";

	void Wand::reload() {
		drawPile.clear();
		hand.clear();
		discardPile.clear();
		for (auto& spell : inventory) {
			if (spell && spell->getKind() != Spell::Kind::UnknownSpell) {
				drawPile.push_back(spell);
			}
		}

		currentReloadDelay = std::max(MIN_CAST_DELAY, currentReloadDelay);
		reloadTimer.set(currentReloadDelay).start();

		currentReloadDelay = this->reloadDelay;
		currentCastDelay = this->castDelay;
	}

	std::string Wand::getWandTextureEntry(int number) {
		if (number < 0 || number > 1000) {
			throw std::out_of_range("Number must be between 0 and 1000.");
		}
		std::ostringstream oss;
		oss << "wand_" << std::setfill('0')
			<< std::setw(4) << number; //".png";
		return oss.str();
	}

	Wand::Wand(float wand_scale) : sprite(*app::Loader::loadTexture(defaultTexture, true)) {
		const int randomNumber = flx::random.nextVal(0, 1000);
		const auto name = vformat("gfx/wands/noita/{}.png", getWandTextureEntry(randomNumber));

		if (const auto t = app::Loader::loadTexture(name)) {
			sprite.setTexture(*t);
		} else {
			logger.warn("Tried to set wand texture {}, but failed, fallback to default.", name);
		}

		const auto textureSize = sprite.getTexture().getSize();
		sprite.setOrigin({0, textureSize.y / 2.0f});
		length = 0.9f * wand_scale * static_cast<float>(textureSize.x);
		sprite.setScale({wand_scale, wand_scale});

		// temp
		castAmount = 1;
		castDelay = 0.05f;
		reloadDelay = 0.1f;

		inventory.resize(10);

		inventory[0] = std::make_shared<AddExplosionSpell>();
		inventory[1] = std::make_shared<MultiShots>(3);
		inventory[2] = std::make_shared<Parasite>();
		inventory[3] = std::make_shared<ScatterShot>();
		inventory[4] = std::make_shared<HomingShot>(6.f);
		inventory.back() = std::make_shared<BulletSpell>();
	}

	void Wand::render(const ui::NPainter& renderer) const {
		renderer.draw(sprite);
	}

	void Wand::cast(const GameCtx& ctx) {
		int drawCount = this->castAmount;
		SpellBlock block;
		bool hasReturned = false;
		while (drawCount > 0) {
			if (!hasReturned && drawPile.empty()) {
				for (const auto& spell : discardPile) {
					drawPile.push_back(spell);
				}
				discardPile.clear();
				hasReturned = true;
			}
			if (!drawPile.empty()) {
				auto& spell = drawPile.front();
				hand.push_back(spell);
				drawCount += spell->drawModifier;
				if (spell->kind == Spell::ProjectileSpell) {
					drawCount--;
				}
				drawPile.pop_front();
			} else {
				break;
			}
		}

		for (const auto& spell : hand) {
			auto type = spell->getKind();
			if (type == Spell::Kind::ProjectileSpell) {
				block.add(spell);
			} else if (type == Spell::Kind::ModifierSpell) {
				block.add(spell);
			} else {
				// handle more types of spells...
			}
			discardPile.push_back(spell);

			currentCastDelay += spell->castDelay;
			currentReloadDelay += spell->reloadDelay;
		}

		hand.clear();

		// std::cout << "cast!\n";
		block.cast(ctx, castPos, arg);

		currentCastDelay = std::max(MIN_CAST_DELAY, currentCastDelay);
		castTimer.set(currentCastDelay).start();
		currentCastDelay = 0.f;

		if (hasReturned || drawPile.empty()) {
			reload();
		}
	}

	void Wand::clear() {
		inventory.clear();
		drawPile.clear();
		discardPile.clear();
		hand.clear();
	}
} // namespace flx::game