#pragma once
#include "src/ui/NObject.h"
#include "src/utils/Pointer.h"

class Spell;
class NSpellInventory;

class NSpell : public NObject {
protected:
	friend class NSpellInventory;
	bool isReleased{};

	n_shared<Spell> spell;
	int index{};

	float rotation{};
	float t{};

	// bool isHovered{};

	void updateTooltipSpec();
	static n_unique<NLayout> tooltipBuilder(const NStyle& style, NObject* self);

public:
	static constexpr nvec2 slotSize = {45.f, 45.f};
	static constexpr float outLine = 3.f;

	explicit NSpell(std::shared_ptr<Spell> spell, nvec2 pos = {});

	std::optional<NEventResult> handleEvent(const NUIEvent& event) override;
	void update(float dt) override;
	void draw(const NPainter& canvas) const override;

	nrect getHitbox() const override {
		return frame.getExpand({5.f, 5.f});
	}

	NSpellInventory* getInventory() const;
};