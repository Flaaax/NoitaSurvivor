#pragma once
#include"src/ecs/entity.h"
#include<deque>

class b2Contact;

class Script {
public:
	bool isDone = false;

	virtual ~Script() {}
	virtual void onUpdate(GameCtx& ctx, myecs::entity self, float dt) {}
	//triggered on removal of the entity
	virtual void onDeath(GameCtx& ctx, myecs::entity self) {}
	virtual void onContact(GameCtx& ctx, myecs::entity self, myecs::entity other, b2Contact* contact) {}
};

struct ScriptComponent {
	std::vector<n_shared<Script>> scripts;
};