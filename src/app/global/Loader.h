#pragma once
#include "src/utils/Macro.h"

#include <filesystem>
#include <string_view>

namespace flx::json {
	class Json;
}

namespace flx::app {
	struct Loader {
		// using FileCallback = std::function<void(const Path& path)>;

		FLX_CONSTEXPR std::string_view ignore_file = ".flx_ignore";
		FLX_CONSTEXPR std::string_view resource_path = "resources/";
		FLX_CONSTEXPR std::string_view texture_path = "resources/gfx/";
		FLX_CONSTEXPR std::string_view data_path = "resources/data/";
		FLX_CONSTEXPR std::string_view texture_config = "config.jsonc";
		FLX_CONSTEXPR bool defaultSmooth = false;

		static const json::Json* loadJson(std::string_view entry, bool required = false);
		static const sf::Texture* loadTexture(std::string_view entry, bool required = false);
		static const sf::Font* loadFont(std::string_view entry, bool required = false);
	};
} // namespace flx::app