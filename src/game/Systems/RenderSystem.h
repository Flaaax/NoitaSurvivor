#include"src/ecs/entity.h"

class Renderer;
class Game;
struct nvec2;
struct GameCtx;

class RenderSystem {
public:
	static void update(float dt, GameCtx& ctx);
	static void render(Renderer& renderer, GameCtx& ctx);
	static void debugRender(const GameCtx& ctx, Renderer& rdr);
};