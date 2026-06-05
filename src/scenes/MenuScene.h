#pragma once

#include "../app/Scene.h"

class MenuScene :public Scene {
public:
	MenuScene();

	void draw(NRenderBuffer& rdr) override;
	void update(float dt) override;
	std::string_view getName() const override;
};