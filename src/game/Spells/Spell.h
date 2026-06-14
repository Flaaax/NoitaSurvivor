#pragma once
#include "src/game/GameContext.h"
#include "src/utils/Tag.h"
#include "src/utils/ID.h"

#pragma warning(disable : 5105)

#ifndef NTS_SPELL_H
#define NTS_SPELL_H

namespace flx::game {
	class Spell {
	public:
		enum Kind {
			UnknownSpell,
			ProjectileSpell,
			ModifierSpell
		};

		struct Loc {
			std::string title;
			std::string description;
			std::string flavor;
		};

		std::string ID;
		Kind kind;

		// template <class T>
		// static std::string_view getID() {
		// 	static std::string ID = Util::makeContentID<T>();
		// 	return ID;
		// }

	public:
		enum class Tag : u64 {
			NO_INHERIT = 1 << 0,
			SHOT_MODIFY = 1 << 1,
		};

		flx::Tag<Tag> tags;

		int drawModifier = 0;
		float castDelay = 0.f;
		float reloadDelay = 0.f;
		float delayMultiplier = 1.f;
		float scatter = 0.f;

		float damage_modifier = 0;
		float speed_modifier = 0;
		float acc_modifier = 0;

		int projectiles = 1;
		float lifeTime = -1.f;

		explicit Spell(std::string_view ID_, Kind kind);
		virtual ~Spell() = default;

		std::string_view getID() const {
			return ID;
		}

		Kind getKind() const {
			return kind;
		}

		virtual const sf::Texture& getTexture() const = 0;
		static const sf::Texture& getTextureFromID(std::string_view ID_);

		virtual const Loc& getLoc() const = 0;
		static Loc makeLocFromID(std::string_view ID_);

		flx::Vector<n_pair<std::string>> getDisplayedProperties() const;
	};

	template <class T, class Base>
	class MakeSpellFromBoilerPlate : public Base {
	public:
		MakeSpellFromBoilerPlate() : Base(flx::makeContentID<T>()) {}

		const Spell::Loc& getLoc() const override {
			static const Spell::Loc loc = Spell::makeLocFromID(Base::getID());
			return loc;
		}

		const sf::Texture& getTexture() const override {
			static const sf::Texture& texture = Spell::getTextureFromID(Base::getID());
			return texture;
		}
	};
} // namespace flx::game

#define DEF_SPELL(T, Base) class T : public ::flx::game::MakeSpellFromBoilerPlate<T, Base>

#endif