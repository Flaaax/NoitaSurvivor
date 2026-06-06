#pragma once

#include"../NObject.h"
#include "NSpellInventory.h"

class NWidget;
class NButton;

class NSpellSelector :public NSpellInventory {
private:
	int selectCount = 0;

	NWidget* ui;

	bool isRunning = true;

	NSpellSelector(const nvec2& center, int selectCount, NWidget* parent);
public:
	~NSpellSelector() override;
	NSpellSelector* create(const nvec2& center, int selectCount);

	void draw(const NPainter& canvas) const override;
	bool getIsRunning()const { return isRunning; }
};