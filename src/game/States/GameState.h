#pragma once
#include "../../utils/Vec2/Vec2.h"
#include "src/ecs/entity.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Pointer.h"
#include "src/utils/Timer.h"

namespace flx::game {
	struct PlayerData {
		myecs::entity id;
		myecs::entity collector;

		bool isShooting = false;

		int material = 0;
		int exp = 0;
		int maxExp = 0;
		int level = 0;

		vec2 controlDir;
		bool isUp{}, isRight{}, isLeft{}, isDown{};
	};

	class Wand;

	struct GameState {
		PlayerData player;

		bool isPaused = false;

		rect bound;
		rect maxBound;
		rect debugEnemySpawnArea;

		vec2 playerPos;
		vec2 mousePos;
		vec2 cameraPos;

		flx::Vector<myecs::entity> borders;
		flx::Vector<Unique<Wand>> wands;

		CTimer enemySpawnTimer;

		bool debugMode = true;
	};
} // namespace flx::game