#pragma once
#include"src/ecs/entity.h"
#include"GameState.h"

class b2World;
class EntityFactory;
class ContactLayerRules;

//Lightweight, stable(its reference), copyable reference of the game context
struct GameCtx {
	myecs::Registry& reg;
	b2World& world;
	EntityFactory& factory;
	ContactLayerRules& contactRules;
	GameState& state;
	const float& debug_dt;
};