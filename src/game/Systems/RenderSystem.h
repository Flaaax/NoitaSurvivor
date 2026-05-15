#include"src/ecs/entity.h"

class Renderer;
class Game;
struct nvec2;
struct GameCtx;

class RenderSystem {
public:
	static void update(const GameCtx& ctx, float dt);
	static void render(Renderer& rdr, GameCtx& ctx);
	static void debugRender(const GameCtx& ctx, Renderer& rdr);
};