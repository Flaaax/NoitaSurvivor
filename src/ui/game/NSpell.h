#pragma once
#include "src/ui/NObject.h"
#include "src/utils/Pointer.h"

class Spell;
class NSpellInventory;

class NSpell : public NObject {
private:
	friend class NSpellInventory;
	bool isReleased{};

	n_shared<Spell> spell;
	int index{};

	float rotation{};
	float t{};

	//bool isHovered{};

public:
	static constexpr nvec2 slotSize = {45.f, 45.f};
	static constexpr float outLine = 3.f;

	explicit NSpell(std::shared_ptr<Spell> spell, nvec2 pos = {});

	std::optional<NEventResult> handleEvent(const NUIEvent& event) override;
	void update(float dt) override;
	void draw(const NCanvas& canvas) const override;

	nrect getHitbox() const override {
		return geometry.getExpand({5.f, 5.f});
	}

	NSpellInventory* getInventory() const;
};