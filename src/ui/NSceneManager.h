#pragma once
#pragma once
#ifndef NSCENEMANAGER_H
#define NSCENEMANAGER_H
#include "../utils/Container/Container.h"
#include <unordered_map>

class NScene;

class NSceneManager {
private:
	Util::StdMap<n_unique<NScene>> scenes;
	NScene* currentScene = nullptr;
	NScene* sceneToChange = nullptr;

public:
	NSceneManager();
	~NSceneManager();

	void addScene(n_unique<NScene> scene);

	NScene* getScene(std::string_view name)const;

	void setCurrentScene(std::string_view name) {sceneToChange = getScene(name);}
	NScene* getCurrentScene()const { return currentScene; }
	bool shouldChangeScene()const { return sceneToChange != nullptr; }

	void changeScene();
};


#endif