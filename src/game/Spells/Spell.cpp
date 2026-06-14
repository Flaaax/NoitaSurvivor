#include "Spell.h"
#include "../../app/global/AssetManager.h"
#include "../../app/global/LocManager.h"
#include "src/utils/Text/Format.h"

#include <magic_enum/magic_enum.hpp>

namespace flx::game {
	Spell::Spell(std::string_view ID_, Kind kind) {
		ID = std::string(ID_);
		this->kind = kind;
	}

	const sf::Texture& Spell::getTextureFromID(std::string_view ID_) {
		return app::AssetMgr::getSpellTexture(ID_);
	}

	Spell::Loc Spell::makeLocFromID(std::string_view ID_) {
		const std::string ID = std::string(ID_);
		constexpr std::string_view entry = "spells";
		return Loc{
			.title = app::LocManager::inst().debugGetString(entry, ID + ".title"),
			.description = app::LocManager::inst().debugGetString(entry, ID + ".description"),
			.flavor = app::LocManager::inst().debugGetString(entry, ID + ".flavor"),
		};
	}

	template <typename T>
	static std::string toDisplayString(T value) {
		if constexpr (std::integral<T>) {
			return std::to_string(value);
		} else if constexpr (std::floating_point<T>) {
			if (std::isfinite(value) && std::abs(value - std::round(value)) < 1e-6) {
				return std::format("{:.0f}", value);
			}

			return std::format("{:.2f}", value);
		}
		throw 1;
	}

#define ADD_DISPLAY_PROPERTY(item, defaultVal) \
	if (item != defaultVal)                    \
		ret.emplace_back(get(text::pascalToSnake(#item)), toDisplayString(item));

#define ITERATE_SPELL_PROPERTIES(fn) \
	fn(drawModifier, 0);             \
	fn(castDelay, 0.f);              \
	fn(reloadDelay, 0.f);            \
	fn(delayMultiplier, 1.f);        \
	fn(scatter, 0.f);                \
	fn(damage_modifier, 0.f);        \
	fn(speed_modifier, 0.f);         \
	fn(acc_modifier, 0.f);           \
	fn(projectiles, 1);              \
	fn(lifeTime, -1.f)

	flx::Vector<n_pair<std::string>> Spell::getDisplayedProperties() const {
		constexpr auto get = [](std::string_view key) {
			return app::LocManager::inst().debugGetString("spell_properties", key, true);
		};
		flx::Vector<n_pair<std::string>> ret{};
		ret.emplace_back(get("kind"), get(text::pascalToSnake(magic_enum::enum_name(kind))));
		ITERATE_SPELL_PROPERTIES(ADD_DISPLAY_PROPERTY);

		return ret;
	}
} // namespace flx::game