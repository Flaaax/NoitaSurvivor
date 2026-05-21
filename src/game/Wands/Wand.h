#pragma once
#include "src/game/GameContext.h"
#include "src/game/Spells/Spell.h"
#include "src/render/Renderer.h"
#include "src/utils/Timer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <deque>

class Wand {
protected:
	sf::Texture texture;
	sf::Sprite sprite;

	void reload();

	static std::string getWandTextureName(int number);

public:
	static constexpr float MIN_CAST_DELAY = 1.f / 60.f;

	//wand
	float castDelay = 0.f;
	float reloadDelay = 0.f;
	float scattering = 0.f;
	int capacity = 0;
	int castAmount = 0;

	bool isReloaded = true;

	float currentCastDelay = 0.f;
	float currentReloadDelay = 0.f;

	Timer castTimer;
	Timer reloadTimer;

	//gfx
	float arg = 0.f;
	float length = 0.f;

	nvec2 worldPos = {};
	nvec2 castPos = {};

	Util::Vector<n_shared<Spell>> inventory;		//Not modified when shooting
	std::deque<n_shared<Spell>> drawPile;
	std::deque<n_shared<Spell>> discardPile;
	std::deque<n_shared<Spell>> hand;

	Wand();

	void setGeometry(nvec2 pos, float arg) {
		worldPos = pos;
		sprite.setPosition(worldPos);
		sprite.setRotation(sf::radians(arg));
		this->arg = arg;
		castPos = worldPos + length * Util::from_rad(arg);
	}

	void render(Renderer& renderer) const {
		renderer.drawGame(sprite);
	}

	void update(float deltaTime) {
		castTimer.update(deltaTime);
		reloadTimer.update(deltaTime);
	}

	void use(const GameCtx& ctx) {
		if (!castTimer.isRunning() && !reloadTimer.isRunning()) {
			cast(ctx);
		}
	}

	void cast(const GameCtx& ctx);

	//Clear all the spells but keep the delays
	void clear();
};

