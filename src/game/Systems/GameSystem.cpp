#pragma warning(disable:5105)
#include "GameSystem.h"
#include"../Components/EntityComponents.h"
#include"../Components/PhysicsComponents.h"
#include"../Components/Script/ScriptComponent.h"
#include <src/utils/Random.h>
#include"src/game/Wands/Wand.h"
#include"../Components/EntityFactory.h"
#include "src/game/Services/EntityService.h"
#include <src/utils/VectorHelper.h>
#include"src/game/GameContext.h"


using namespace myecs;

void GameSystem::update(float dt, GameCtx& ctx) {
	auto& reg = ctx.reg;
	{
		const auto mydir = ctx.gameState.mousePos - ctx.gameState.playerPos;
		auto& wand = *ctx.gameState.wands.front();
		wand.setGeometry(ctx.gameState.playerPos, mydir.rad());
		wand.update(dt);
		if (ctx.gameState.player.isShooting) {
			wand.use(ctx);
		}
	}

	for (auto [e, s] : reg.view<ScriptComponent>()) {
		for (const auto& sc : s.scripts) {
			sc->onUpdate(ctx, e, dt);
		}
		Util::eraseIf(s.scripts, [](const n_shared<Script>& ele) {
			return ele->isDone;
		});
	}

	for (auto [e, dc] : reg.view<DirectionComponent>()) {
		dc.dir = {};
	}

	for (auto [e, c] : reg.view<LifetimeComponent>()) {
		c.lifeTimer.update(dt);
		if (!c.lifeTimer.isRunning()) {
			EntityService::kill(ctx, e);
		}
	}

	for (const auto& [e, c] : reg.view<EnemyComponent>()) {
		c.impulse = {};
	}
}

void GameSystem::handleEvent(const sf::Event& event, GameCtx& ctx) {
	auto& p = ctx.gameState.player;
	bool updateDir = false;
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.key.code == sf::Mouse::Left) p.isShooting = true;
	}
	else if (event.type == sf::Event::MouseButtonReleased) {
		if (event.key.code == sf::Mouse::Left) p.isShooting = false;
	}
	else if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::W) { p.isUp = true; updateDir = true; }
		else if (event.key.code == sf::Keyboard::S) { p.isDown = true; updateDir = true; }
		else if (event.key.code == sf::Keyboard::A) { p.isLeft = true; updateDir = true; }
		else if (event.key.code == sf::Keyboard::D) { p.isRight = true; updateDir = true; }
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::W) { p.isUp = false; updateDir = true; }
		else if (event.key.code == sf::Keyboard::S) { p.isDown = false; updateDir = true; }
		else if (event.key.code == sf::Keyboard::A) { p.isLeft = false; updateDir = true; }
		else if (event.key.code == sf::Keyboard::D) { p.isRight = false; updateDir = true; }
	}
	else if (event.type == sf::Event::LostFocus) {
		p.isUp = p.isLeft = p.isDown = p.isRight = false;
		updateDir = true;
	}

	if (updateDir) {
		p.controlDir.x = (p.isRight ? 1.F : 0.F) - (p.isLeft ? 1.F : 0.F);
		p.controlDir.y = (p.isDown ? 1.F : 0.F) - (p.isUp ? 1.F : 0.F);
		p.controlDir.normalize();
		//Logger::info("Dir updated, cur= {}, {}", p.controlDir.x, p.controlDir.y);
	}

}
