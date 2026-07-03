#pragma once
#include "src/utils/Container/Optional.h"
#include "src/utils/Macro.h"

#include <filesystem>
#include <string_view>

namespace flx::fon {
	class Fon;
}

namespace flx::app {
	struct Loader {
		// using FileCallback = std::function<void(const Path& path)>;

		FLX_CONSTEXPR std::string_view ignore_file = ".flx_ignore";
		FLX_CONSTEXPR std::string_view resource_path = "resources/";
		FLX_CONSTEXPR std::string_view texture_config = "config.jsonc";
		FLX_CONSTEXPR bool defaultSmooth = false;

		static Optional<const fon::Fon&> loadFonFile(std::string_view entry);
		static Optional<const sf::Texture&> loadTexture(std::string_view entry);
		static Optional<const sf::Font&> loadFont(std::string_view entry);
	};
} // namespace flx::app