#pragma once
#include"src/ecs/entity.h"
#include "States/GameState.h"
#include"States/ContactState.h"
#include<box2d/types.h>


class EntityFactory;
class ContactLayerRules;

struct WorldCtx {
	b2WorldId world = b2_nullWorldId;
	bool isStepping{};
};

//Lightweight, copyable reference of the game context
struct GameCtx {
	myecs::Registry& reg;
	const WorldCtx& worldCtx;
	EntityFactory& factory;
	const ContactLayerRules& contactRules;
	GameState& gameState;
	ContactState& contactState;
};