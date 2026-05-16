#pragma once
#include "src/ui/NObject.h"

struct NSpellSlot;
class Spell;
class NSpell :public NObject {
private:
	friend class NSpellInventory;
	enum Stat {
		None,
		Dragged,
		Released
	}stat;

	std::shared_ptr<Spell> spell;
	NSpellSlot* slot{};

	float rotation{};
	float t{};

	static constexpr float slotSize = 45.f;
	static constexpr float outLine = 3.f;

public:
	NSpell(std::shared_ptr<Spell> spell, const nvec2& pos = {});

	bool handleEvent(const sf::Event& event)override;
	void update(float dt)override;
	void draw(Renderer& renderer)const override;


	nrect getHitbox()const { return m_geometry.getExpand({ 15.f,15.f }); }
};