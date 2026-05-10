#pragma once
#pragma warning(push)
#pragma warning(disable : 4251)
#include"../utils/Logger.h"
#include"../utils/Macro.h"
#include"src/utils/Container.h"

namespace std { namespace filesystem { class path; } }
namespace sf { class Font; class Texture; class SoundBuffer; class Sound; }


class AssetMgr {
public:
	static const sf::Font& getDefaultFont();
	static const sf::Font& getFont(std::string_view name);
	static void playSound(std::string_view name, int index = -1);
	static const sf::Texture& getSpellTexture(std::string_view name);
	static const sf::Texture& getUITexture(std::string_view name);
	static const sf::Texture& getWandTexture(std::string_view name);
	static const sf::Texture& getSpriteTexture(std::string_view name);
	static void init();
};


#pragma warning(pop)
