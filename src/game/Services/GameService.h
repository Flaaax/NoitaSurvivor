#pragma once
#include"src/utils/Vec2.h"

struct GameCtx;

class GameService {
public:
	static void dropMaterial(const GameCtx& ctx, nvec2 pos);

};