#include"src/ecs/entity.h"

class Renderer;
class Game;
struct nvec2;
struct GameCtx;

class RenderSystem {
public:
	void update(float dt, GameCtx& ctx);
	void render(Renderer& renderer, GameCtx& ctx);
	void debugRender(Renderer& renderer, GameCtx& ctx);
};