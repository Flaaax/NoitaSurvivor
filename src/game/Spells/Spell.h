#pragma once
#include<SFML/Graphics/Texture.hpp>
#include"src/utils/Tag.h"
#include<optional>
#include"src/game/GameContext.h"
#include"src/ecs/types.h"
//#include"src/global/Register.h"

#ifndef NOITASURVIVOR_SPELL_H
#define NOITASURVIVOR_SPELL_H

#pragma warning(disable:5105)

class Spell {
private:
	std::optional<sf::Texture> texture;
protected:
	template<class T>
	constexpr static std::string_view getID() {
		return myecs::types::type_name<T>().substr(6);
	}

public:
	virtual ~Spell() = default;

	enum SpellType {
		PROJECTILE_SPELL,
		MODIFIER_SPELL,
		OTHER_SPELL
	};

	enum class Tag :size_t {
		NO_INHERIT = 1 << 0,
		SHOT_MODIFY = 1 << 1,
	};

	int drawModifier = 0;

	float castDelay = 0.f;
	float reloadDelay = 0.f;
	float delayFactor = 1.f;

	Util::Tag<Tag> tags;
	std::string_view ID{};

	virtual SpellType getSpellType() const = 0;

	sf::Texture& getTexture();
};

#endif