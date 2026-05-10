#include"GameStateSystem.h"
#include"../GameContext.h"
#include"src/game/Components/PhysicsComponents.h"
#include"src/utils/Pointer.h"
#include"src/game/Wands/Wand.h"
#include"src/game/Components/EntityFactory.h"
#include"src/utils/VectorHelper.h"


void GameStateSystem::initStates(GameCtx& ctx) {
	using namespace Util;

	auto& state = ctx.state;

	state.wands += make_unique(new Wand());
	
	state.player.id = ctx.factory.createPlayer();
	state.player.collector = ctx.factory.createCollector(3.f);
	state.player.maxExp = 20;

	ctx.factory.createMaterial({ 6.f,6.f });

	//test
	state.enemySpawnTimer.set(1.0f, [factory=&ctx.factory] {
		factory->createEnemy({ 0,0 });
	}).start(-1);

	nrect bound = state.bound;

	std::vector<std::pair<nvec2, nvec2>> borderEdges = {
		{bound.topLeft(),		bound.topRight()	},
		{bound.topRight(),		bound.bottomRight()	},
		{bound.bottomRight(),	bound.bottomLeft()	},
		{bound.bottomLeft(),	bound.topLeft()		}
	};
	for (auto& edge : borderEdges) {
		state.borders += ctx.factory.createBorder(edge.first, edge.second);
	}
}

void GameStateSystem::updateBeforePhysics(GameCtx& ctx) {
	auto& state = ctx.state;
	state.mousePos = (NWindow::mouseRenderPos - NWindow::scale.gameRenderOffset) / NWindow::scale.gameRenderScale + state.cameraPos;
	myecs::entity player = ctx.state.player.id;
	auto& playerBody = ctx.reg.get<BodyComponent>(player);
	state.playerPos = playerBody.getPosition();
	state.cameraPos = state.playerPos;
}
