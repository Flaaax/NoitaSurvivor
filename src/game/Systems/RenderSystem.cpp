#include"../Components/Render/RenderComponent.h"
#include"RenderSystem.h"
#include"src/gui/Renderer.h"
#include"src/gui/shapes/NLineShape.h"
#include<src/game/Components/PhysicsComponents.h>
#include"src/global/DataManager.h"
#include"src/utils/Assert.h"
#include"src/game/Wands/Wand.h"


void RenderSystem::debugRender(Renderer& rdr, GameCtx& ctx) {
	NLineShape shape;
	for (const auto& e : ctx.gameState.borders) {
		if (auto b = ctx.reg.try_get<BodyComponent>(e)) {
			auto edgeShape = static_cast<b2EdgeShape*>(b->body->GetFixtureList()->GetShape());
			shape.set(edgeShape->m_vertex1, edgeShape->m_vertex2, { 200,0,0 });
			rdr.drawGame(shape);
		}
	}
}

void RenderSystem::update(float dt, GameCtx& ctx) {
	for (const auto& [e, tc] : ctx.reg.view<SpriteEffectComponent>()) {
		for (auto& effect : tc.effectList) {
			effect->update(dt);
		}
	}
}

void RenderSystem::render(Renderer& rdr, GameCtx& ctx) {
	rdr.updateGameRender(ctx.gameState.cameraPos * NWindow::scale.gameRenderScale - NWindow::scale.gameRenderOffset);

	rdr.clear(sf::Color(100, 100, 100));

	sf::RectangleShape arenaShape;
	arenaShape.setSize(ctx.gameState.bound.size);
	arenaShape.setFillColor(sf::Color(170, 170, 170));
	arenaShape.setPosition(ctx.gameState.bound.position);
	rdr.drawGame(arenaShape);

	if (ctx.gameState.debugMode) {
		sf::RectangleShape testShape;
		testShape.setSize({ 4,4 });
		testShape.setFillColor({ 100,100,100 });
		testShape.setPosition(0, 0);
		rdr.drawGame(testShape);
	}

	//Logger::info("begin render");
	for (const auto& [e, c, bc] : ctx.reg.view<SpriteComponent, BodyComponent>()) {
		auto& c = ctx.reg.get<SpriteComponent>(e);
		//restore to default state
		sf::Sprite old = *c.sprite;

		if (c.info->followPosition)
			c.sprite->setPosition(bc.getPosition() + c.info->positionOffset);
		if (c.info->followAngle)
			c.sprite->setRotation(Util::to_deg(bc.body->GetAngle()) + c.info->rotationOffset);
		if (c.info->dynamicScale) {
			float r = bc.getRadius();
			c.sprite->scale(r, r);
		}

		if (auto tc = ctx.reg.try_get<SpriteEffectComponent>(e)) {
			auto& ts = tc->effectList;
			for (auto it = ts.begin(); it != ts.end();) {
				(*it)->apply(*c.sprite);

				if ((*it)->isDone()) {
					it = ts.erase(it);
				}
				else it++;
			}
		}

		rdr.drawGame(*c.sprite);

		*c.sprite = old;
	}

	for (auto& wand : ctx.gameState.wands) {
		wand->render(rdr);
	}

	if (ctx.gameState.debugMode) {
		debugRender(rdr, ctx);
	}
}
