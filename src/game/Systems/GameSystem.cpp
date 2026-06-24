#pragma warning(disable : 5105)
#include "GameSystem.h"
#include "../Components/EntityComponents.h"
#include "../Components/EntityFactory.h"
#include "../Components/Script/ScriptComponent.h"
#include "src/game/GameContext.h"
#include "src/game/Services/EntityService.h"
#include "src/game/Wands/Wand.h"
#include <src/utils/VectorHelper.h>

namespace flx::game {
	using namespace myecs;

	void GameSystem::update(const GameCtx& ctx, float dt) {
		auto& reg = ctx.reg;

		ctx.gameState.wandManager.update(ctx, dt);

		// {
		// 	const auto mydir = ctx.gameState.mousePos - ctx.gameState.playerPos;
		// 	auto& wand = *ctx.gameState.wands.front();
		// 	wand.setGeometry(ctx.gameState.playerPos, mydir.rad());
		// 	wand.update(dt);
		// 	if (ctx.gameState.player.isShooting) {
		// 		wand.use(ctx);
		// 	}
		// }

		for (auto [e, s] : reg.view<ScriptComponent>()) {
			for (const auto& sc : s.scripts) {
				sc->onUpdate(ctx, e, dt);
			}
			flx::eraseIf(s.scripts, [](const Shared<Script>& ele) {
				return ele->isDone;
			});
		}

		for (auto [e, c] : reg.view<LifetimeComponent>()) {
			c.lifeTimer.update(dt);
			if (!c.lifeTimer.isRunning()) {
				EntityService::kill(ctx, e);
			}
		}
	}

	void GameSystem::handleEvent(const GameCtx& ctx, const sf::Event& event) {
		auto& p = ctx.gameState.player;
		bool updateDir = false;
		if (const auto e1 = event.getIf<sf::Event::MouseButtonPressed>()) {
			if (e1->button == sf::Mouse::Button::Left) {
				p.isShooting = true;
			}
		} else if (const auto e2 = event.getIf<sf::Event::MouseButtonReleased>()) {
			if (e2->button == sf::Mouse::Button::Left) {
			}
			p.isShooting = false;
		} else if (const auto e3 = event.getIf<sf::Event::KeyPressed>()) {
			if (e3->code == sf::Keyboard::Key::W) {
				p.isUp = true;
				updateDir = true;
			} else if (e3->code == sf::Keyboard::Key::S) {
				p.isDown = true;
				updateDir = true;
			} else if (e3->code == sf::Keyboard::Key::A) {
				p.isLeft = true;
				updateDir = true;
			} else if (e3->code == sf::Keyboard::Key::D) {
				p.isRight = true;
				updateDir = true;
			}
		} else if (const auto e4 = event.getIf<sf::Event::KeyReleased>()) {
			if (e4->code == sf::Keyboard::Key::W) {
				p.isUp = false;
				updateDir = true;
			} else if (e4->code == sf::Keyboard::Key::S) {
				p.isDown = false;
				updateDir = true;
			} else if (e4->code == sf::Keyboard::Key::A) {
				p.isLeft = false;
				updateDir = true;
			} else if (e4->code == sf::Keyboard::Key::D) {
				p.isRight = false;
				updateDir = true;
			}
		} else if (event.is<sf::Event::FocusLost>()) {
			p.isUp = p.isLeft = p.isDown = p.isRight = false;
			updateDir = true;
		}

		if (updateDir) {
			p.controlDir.x = (p.isRight ? 1.F : 0.F) - (p.isLeft ? 1.F : 0.F);
			p.controlDir.y = (p.isUp ? 1.F : 0.F) - (p.isDown ? 1.F : 0.F);
			p.controlDir.normalize();
			// Logger::info("Dir updated, cur= {}, {}", p.controlDir.x, p.controlDir.y);
		}
	}

	void GameSystem::updateAfterCleanup(const GameCtx& ctx) {
		auto& reg = ctx.reg;
		for (const auto& [e, c] : reg.view<EnemyComponent>()) {
			c.impulseRecieved = {};
		}
		for (auto [e, dc] : reg.view<DirectionComponent>()) {
			dc.dir = {};
		}
	}
} // namespace flx::game