#pragma once
#include "NScale.h"
#include "NSceneManager.h"
#include "context/NInputState.h"
#include "src/utils/Macro.h"
#include "src/utils/Singleton.h"
#include "src/utils/Vec2.h"

namespace sf { class RenderWindow; }

//manage the whole application, should only have 1 instance
class NWindow {
	N_DECL_SINGLETON(NWindow);
private:
	n_unique<sf::RenderWindow> window;

	void updateWindowSize() const;
	void updateMousePos() const;

public:
	NInputState inputState;

	N_STATIC_VAR NScale scale;
	N_STATIC_VAR NSceneManager sceneManager;

	NWindow();
	~NWindow();
	//should only run once
	int loop() const;

	static sf::RenderWindow& getWindow() { return *inst().window; }
};