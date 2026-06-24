#pragma once

namespace flx::ui {
	class Painter;
	class RenderBuffer;
} // namespace flx::ui

namespace flx::game {
	class Game;
	struct GameCtx;

	class RenderSystem {
	public:
		static void update(const GameCtx& ctx, float dt);
		static void render(ui::RenderBuffer& buffer, const GameCtx& ctx);
		static void debugRender(const GameCtx& ctx, const ui::Painter& rdr);
	};
} // namespace flx::game