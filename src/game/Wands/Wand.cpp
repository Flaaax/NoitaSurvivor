// #include"src/game/Spells/Modifiers/AddExplosionSpell.h"
#include "Wand.h"
#include "src/app/global/Loader.h"
#include "src/game/Contact/ContactLayerRules.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Services/PhysicsService.h"
#include "src/game/Spells/Modifiers/BasicModifiers.h"
#include "src/game/Spells/Projectiles/BasicProjectiles.h"
#include "src/game/Spells/SpellBlock.h"
#include "src/ui/render/Painter.h"
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

		currentReloadDelay = std::max(minCastDelay, currentReloadDelay);
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

	Wand::Wand(float wandScale) {
		const int randomNumber = flx::random.nextVal(0, 1000);
		const auto name = vformat("gfx/wands/noita/{}.png", getWandTextureEntry(randomNumber));

		texture = app::Loader::loadTexture(name, true);

		const auto textureSize = texture->getSize();
		// sprite.setOrigin({0, textureSize.y / 2.0f});
		length = 0.9f * wandScale * static_cast<float>(textureSize.x);
		holdLength = length * 0.33f;
		scale = {wandScale, wandScale};
		// sprite.setScale({wandScale, wandScale});

		// temp
		castAmount = 1;
		castDelay = 0.05f;
		reloadDelay = 0.1f;

		inventory.resize(10);

		// inventory[0] = std::make_shared<AddExplosionSpell>();
		// inventory[1] = std::make_shared<MultiShots>(3);
		inventory[2] = std::make_shared<Parasite>();
		// inventory[3] = std::make_shared<ScatterShot>();
		inventory[4] = std::make_shared<HomingShot>(6.f);
		inventory.back() = std::make_shared<BulletSpell>();
	}

	void Wand::updateGeometry(float dt) {
		if (autoAim) {
			const float diff = math::argDiff(targetRot, currentRot);
			const float sign = math::sgn(diff);
			const float offset = dt * rotSpeed;
			if (std::abs(diff) <= offset) {
				currentRot = targetRot;
			} else {
				currentRot += sign * offset;
			}
		} else {
			currentRot = targetRot;
		}

		const auto dir = vec2::rad(currentRot);
		spritePosition = worldPos - holdLength * dir;
		castPos = worldPos + (length - holdLength) * dir;
	}

	void Wand::updateTarget(const GameCtx& ctx) {
		const auto playerPos = ctx.gameState.playerPos;

		worldPos = playerPos + localPos;

		if (autoAim) {
			if (!target) {
				target = PhysicsService().queryNearestEntity(ctx, EntityType::Enemy, worldPos, aimRadius, target, true);
			}
			if (!EntityService().isValidAndAlive(ctx, target)) {
				target = {};
			}
			if (target) {
				const auto targetPos = PhysicsService().getPosition(ctx, target);
				targetRot = (targetPos - worldPos).rad();
			}
		} else {
			targetRot = (ctx.gameState.mousePos - worldPos).rad();
		}
	}

	void Wand::draw(const ui::Painter& renderer) const {
		auto sprite = sf::Sprite(*texture);
		const auto textureSize = texture->getSize();
		sprite.setOrigin({0, textureSize.y / 2.0f});
		sprite.setPosition(spritePosition);
		sprite.setRotation(sf::radians(currentRot));
		sprite.setScale(scale);

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
			const auto kind = spell->getKind();
			if (kind == Spell::Kind::ProjectileSpell) {
				block.add(spell);
			} else if (kind == Spell::Kind::ModifierSpell) {
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
		block.cast(ctx, castPos, targetRot);

		currentCastDelay = std::max(minCastDelay, currentCastDelay);
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