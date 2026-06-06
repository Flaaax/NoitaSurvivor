#pragma once

class NPainter;
class NRenderBuffer;
class Game;
struct nvec2;
struct GameCtx;

class RenderSystem {
public:
	static void update(const GameCtx& ctx, float dt);
	static void render(NRenderBuffer& buffer, const GameCtx& ctx);
	static void debugRender(const GameCtx& ctx, const NPainter& rdr);
};