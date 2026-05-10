#pragma once
#include"ModifierSpell.h"
#include"src/utils/Math.h"


class AddExplosionSpell : public ModifierSpell {
public:
	AddExplosionSpell() {
		castDelay = 0.2f;
		reloadDelay = 0.3f;
		ID = getID<AddExplosionSpell>();
	}

	void apply(GameCtx& ctx, myecs::entity p) override;
};


class MultiShots : public ModifierSpell {
private:
	int shots;

public:
	MultiShots(int shots = 2) {
		this->shots = shots;
		castDelay = 0.1f;
		reloadDelay = 0.1f;
		tags.add(Spell::Tag::SHOT_MODIFY);

		ID = getID<MultiShots>();
	}

	void modifyShot(std::vector<ShotData>& data) override;
};


class ScatterShot : public ModifierSpell {
public:
	ScatterShot() {
		castDelay = -0.3f;
		reloadDelay = -0.5f;
		scatter = 3.f * Util::PI;

		ID = getID<ScatterShot>();
	}
};


class HomingShot : public ModifierSpell {
public:
	float radius;

	HomingShot(float radius = 4.f) : radius(radius) {
		castDelay += 0.2f;
		reloadDelay += 0.4f;

		ID = getID<HomingShot>();
	}

	void apply(GameCtx& ctx, myecs::entity p) override;
};


class Script;

class Parasite : public ModifierSpell {
private:
	std::shared_ptr<Script> script;

public:
	Parasite() {
		delayFactor = 2.f;

		ID = getID<Parasite>();
	}

	void apply(GameCtx& ctx, myecs::entity p) override;
};