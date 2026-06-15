#pragma once
#pragma warning(push)
#include "Loader.h"
#include "src/utils/Macro.h"
#include "src/utils/Vec2/Vec2.h"

namespace sf {
	class Font;
	class Texture;
} // namespace sf

namespace flx::app {
	struct TextureDef {
		sf::Texture texture;
		vec2 origin = {}; // Apply before scale
		vec2 scale = {1.f, 1.f};
	};

	class AssetMgr {
	public:
		FLX_CONSTEXPR std::string_view ignoreFile = ".flx_ignore";
		FLX_CONSTEXPR std::string_view texturePath = "./resources/gfx/";

		static const sf::Font& getDefaultFont();
		static const sf::Font& getFont(std::string_view name);
		static void playSound(std::string_view name, int index = -1);
		static const sf::Texture& getTexture(std::string_view entry, std::string_view name);
		static const TextureDef& getTextureDef(std::string_view entry, std::string_view name);
		static const AssetTable<TextureDef>& getTextureTable(std::string_view entry);
		static void init(); // Optional
	};
} // namespace flx::app

#pragma warning(pop)
