#pragma once
#include "src/ecs/entity.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"
#include "src/utils/Timer.h"
#include "src/utils/Vec2.h"

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

	nrect bound = {0, 0, 40, 40, nrect::CenterRect};
	nrect maxBound = {0, 0, 80, 80, nrect::CenterRect};
	nrect debugEnemySpawnArea = {0, 0, 35, 35, nrect::CenterRect};

	nvec2 playerPos;
	nvec2 mousePos;
	nvec2 cameraPos;

	Util::Vector<myecs::entity> borders;
	Util::Vector<n_unique<Wand>> wands;

	CTimer enemySpawnTimer;

	bool debugMode = true;
};