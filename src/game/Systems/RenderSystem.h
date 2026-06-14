#pragma once

namespace flx::ui {
	class NPainter;
	class NRenderBuffer;
} // namespace flx::ui

namespace flx::game {
	class Game;
	struct GameCtx;

	class RenderSystem {
	public:
		static void update(const GameCtx& ctx, float dt);
		static void render(ui::NRenderBuffer& buffer, const GameCtx& ctx);
		static void debugRender(const GameCtx& ctx, const ui::NPainter& rdr);
	};
} // namespace flx::game