#pragma once
#include"src/ecs/entity.h"
#include "States/GameState.h"
#include"States/ContactState.h"


class b2World;
class EntityFactory;
class ContactLayerRules;

//Lightweight, stable(its reference), copyable reference of the game context
struct GameCtx {
	myecs::Registry& reg;
	b2World& world;
	EntityFactory& factory;
	const ContactLayerRules& contactRules;
	GameState& gameState;
	ContactState& contactState;
};