#pragma once
#include "ModifierSpell.h"
#include "src/utils/Math.h"

class AddExplosionSpell : public ModifierSpell {
public:
	AddExplosionSpell() {
		castDelay = 0.2f;
		reloadDelay = 0.3f;
		ID = getID<AddExplosionSpell>();
	}

	void apply(const GameCtx& ctx, myecs::entity p) override;
};

class MultiShots : public ModifierSpell {
private:
	int shots = 2;

public:
	explicit MultiShots(int shots = 2);

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

	explicit HomingShot(float radius = 4.f) : radius(radius) {
		castDelay += 0.2f;
		reloadDelay += 0.4f;

		ID = getID<HomingShot>();
	}

	void apply(const GameCtx& ctx, myecs::entity p) override;
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

	void apply(const GameCtx& ctx, myecs::entity p) override;
};