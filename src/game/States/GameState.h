#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/ecs/entity.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"
#include "src/utils/Timer.h"

struct PlayerData {
	myecs::entity id;
	myecs::entity collector;

	bool isShooting = false;

	int material = 0;
	int exp = 0;
	int maxExp = 0;
	int level = 0;

	nvec2 controlDir;
	bool isUp{}, isRight{}, isLeft{}, isDown{};
};

class Wand;

struct GameState {
	PlayerData player;

	bool isPaused = false;

	nrect bound;
	nrect maxBound;
	nrect debugEnemySpawnArea;

	nvec2 playerPos;
	nvec2 mousePos;
	nvec2 cameraPos;

	Util::Vector<myecs::entity> borders;
	Util::Vector<n_unique<Wand>> wands;

	CTimer enemySpawnTimer;

	bool debugMode = true;
};