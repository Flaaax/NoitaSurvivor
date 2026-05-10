#pragma once
#include"src/utils/Vec2.h"
#include"NScale.h"
#include"src/utils/Singleton.h"
#include"src/utils/Macro.h"
#include"NSceneManager.h"


namespace sf { class RenderWindow; }

//manage the whole application, should only have 1 instance
class NWindow {
	N_DECL_SINGLETON(NWindow);
private:
	n_unique<sf::RenderWindow> window;

	void updateWindowSize();
	void updateMousePos();

public:
	N_STATIC_VAR nvec2 mouseRealPos = {};
	N_STATIC_VAR nvec2 mouseRenderPos = {};

	N_STATIC_VAR NScale scale;
	N_STATIC_VAR NSceneManager sceneManager;

	NWindow();
	~NWindow();
	//should only run once
	int loop();

	static sf::RenderWindow& getWindow() { return *inst().window; }
};