#pragma once

#include "../ui/NScene.h"

class MenuScene :public NScene {
public:
	MenuScene();

	void draw(Renderer& rdr) override;
	void update(float dt) override;
	std::string_view getName() const override;
};