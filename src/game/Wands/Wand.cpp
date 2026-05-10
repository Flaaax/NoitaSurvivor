//#include"src/game/Spells/Modifiers/AddExplosionSpell.h"
#include"src/game/Spells/Modifiers/Modifiers.h"
#include"src/game/Spells/Projectiles/Projectiles.h"
#include"src/gui/NWindow.h"
#include"Wand.h"
#include<iomanip>
#include<sstream>
#include"src/utils/Logger.h"
#include"src/global/AssetManager.h"
#include"src/game/Spells/SpellBlock.h"
#include"src/utils/Random.h"


void Wand::reload() {
	drawPile.clear();
	hand.clear();
	discardPile.clear();
	for (auto& spell : inventory) {
		if (spell && spell->getSpellType() != Spell::SpellType::OTHER_SPELL) {
			drawPile.push_back(spell);
		}
	}

	currentReloadDelay = Util::max(MIN_CAST_DELAY, currentReloadDelay);
	reloadTimer.set(currentReloadDelay).start();

	currentReloadDelay = this->reloadDelay;
	currentCastDelay = this->castDelay;
}

std::string Wand::getWandTextureName(int number) {
	if (number < 0 || number > 1000) {
		throw std::out_of_range("Number must be between 0 and 1000.");
	}
	std::ostringstream oss;
	oss << "noita_wand_" << std::setfill('0')
		<< std::setw(4) << number; //".png";
	return oss.str();
}

Wand::Wand() {
	int randomNumber = Util::random.get(0, 1000);
	auto name = getWandTextureName(randomNumber);
	texture = AssetMgr::getWandTexture(name);

	sprite.setTexture(texture);
	auto textureSize = texture.getSize();
	sprite.setOrigin(0, textureSize.y / 2.0f);
	auto wand_scale = NWindow::scale.gfx_wand_scale;
	length = 0.9f * wand_scale * static_cast<float>(textureSize.x);
	sprite.setScale(wand_scale, wand_scale);

	//temp
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

void Wand::cast(GameCtx& ctx) {
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
			drawPile.pop_front();
		}
		else {
			break;
		}
	}

	for (const auto& spell : hand) {
		auto type = spell->getSpellType();
		if (type == Spell::SpellType::PROJECTILE_SPELL) {
			block.add(spell);
		}
		else if (type == Spell::SpellType::MODIFIER_SPELL) {
			block.add(spell);
		}
		else {
			//handle more types of spells...
		}
		discardPile.push_back(spell);

		currentCastDelay += spell->castDelay;
		currentReloadDelay += spell->reloadDelay;
	}

	hand.clear();

	//std::cout << "cast!\n";
	block.cast(ctx, castPos, arg);

	currentCastDelay = Util::max(MIN_CAST_DELAY, currentCastDelay);
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