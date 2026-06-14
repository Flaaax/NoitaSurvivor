#pragma warning(disable : 5105)
#include "AssetManager.h"

#include "src/utils/Container/Map.h"
#include "src/utils/Logging/Logger.h"
#include "src/utils/Macro.h"
#include "src/utils/Singleton.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <regex>
#include <src/utils/Random.h>

namespace flx::app {
	class AssetMgrImpl {
		FLX_DECL_SINGLETON(AssetMgrImpl);
		void loadTextures();
		void loadFonts();
		void loadSounds();

	public:
		FLX_CONSTEXPR auto resource_path = "./resources";
		FLX_CONSTEXPR auto font_path = "./resources/fonts";
		FLX_CONSTEXPR auto sound_path = "./resources/sounds";
		FLX_CONSTEXPR auto default_font = "msyh";
		FLX_CONSTEXPR auto spell_gfx_path = "./resources/gfx/spells";			  // 32*32
		FLX_CONSTEXPR auto spell_noita_gfx_path = "./resources/gfx/spells/noita"; // 16*16
		FLX_CONSTEXPR auto spell_gfx_default = "default";

		flx::StrMap<sf::Font> fonts;
		flx::StrMap<std::vector<std::unique_ptr<sf::SoundBuffer>>> soundBuffers;

		flx::StrMap<sf::Texture> spellTextures;
		flx::StrMap<sf::Texture> UITextures;
		flx::StrMap<sf::Texture> wandTextures;
		flx::StrMap<sf::Texture> spriteTextures;

		std::vector<std::unique_ptr<sf::Sound>> sounds;

		AssetMgrImpl() {
			loadTextures();
			loadFonts();
			loadSounds();
		}
	};

	static AssetMgrImpl& inst() {
		return AssetMgrImpl::inst();
	}

	namespace fs = std::filesystem;

	static void initTexture(flx::StrMap<sf::Texture>& textures, std::string& name, const std::filesystem::path& path) {
		if (const auto it = textures.find(name); it != textures.end()) {
			logger.error_and_throw("duplicated texture name: {} with path {}", name, path.string());
			return;
		}
		auto& t = textures[std::move(name)];
		if (!t.loadFromFile(path.string())) {
			logger.error_and_throw("failed to load texture {} from path {}", name, path.string());
		}
		t.setSmooth(false);
	}

	const sf::Font& AssetMgr::getDefaultFont() {
		return inst().fonts[AssetMgrImpl::default_font];
	}

	const sf::Font& AssetMgr::getFont(std::string_view name) {
		if (const auto it = inst().fonts.find(name); it != inst().fonts.end()) {
			return it->second;
		}
		logger.warn("font {} does not exist", name);
		return getDefaultFont();
	}

	void AssetMgr::playSound(std::string_view name, int index) {
		if (inst().sounds.size() >= 50) {
			logger.warn("Too many sounds!");
			return;
		}

		if (const auto it = inst().soundBuffers.find(name); it != inst().soundBuffers.end()) {
			if (index == -1) {
				index = flx::random.nextVal<int>(0, static_cast<int>(it->second.size()) - 1);
			}

			for (auto& sound : inst().sounds) {
				if (sound->getStatus() != sf::Sound::Status::Playing) {
					sound->setBuffer(*it->second[static_cast<size_t>(index)]);
					sound->play();
					return;
				}
			}

			auto newSound = std::make_unique<sf::Sound>(*it->second[static_cast<size_t>(index)]);
			newSound->setVolume(50.f);
			newSound->play();
			inst().sounds.emplace_back(std::move(newSound));
		} else {
			logger.warn("Cant find sound: {}", name);
		}
	}

	const sf::Texture& AssetMgr::getSpellTexture(std::string_view name) {
		if (const auto it = inst().spellTextures.find(name); it != inst().spellTextures.end()) {
			return it->second;
		}
		logger.warn("Spell texture '{}' does not exist", name);
		return inst().spellTextures[inst().spell_gfx_default];
	}

	const sf::Texture& AssetMgr::getUITexture(std::string_view name) {
		if (const auto it = inst().UITextures.find(name); it != inst().UITextures.end()) {
			return it->second;
		}
		return getSpellTexture("default");
	}

	const sf::Texture& AssetMgr::getWandTexture(std::string_view name) {
		if (const auto it = inst().wandTextures.find(name); it != inst().wandTextures.end()) {
			return it->second;
		}
		return inst().wandTextures["noita_wand_0000"];
	}

	const sf::Texture& AssetMgr::getSpriteTexture(std::string_view name) {
		if (const auto it = inst().spriteTextures.find(name); it != inst().spriteTextures.end()) {
			return it->second;
		}
		return inst().spriteTextures["default"];
	}

	// input: aaa_bbb_c_N.wav, output: aaa_bbb_c
	static std::string remove_number_suffix(const std::string& filename) {
		std::regex pattern(R"((.*)_\d+\.wav$)");
		std::smatch match;

		if (std::regex_match(filename, match, pattern)) {
			return match[1];
		}
		return filename;
	}

	void AssetMgrImpl::loadSounds() {
		for (const auto& entry : fs::recursive_directory_iterator(sound_path)) {
			auto extension = entry.path().extension();
			if (entry.is_regular_file() && extension == ".wav") {
				auto buffer = std::make_unique<sf::SoundBuffer>();
				if (!buffer->loadFromFile(entry.path().string())) {
					auto err = std::format("Failed to load sound: {}", entry.path().string());
					throw std::runtime_error(err);
				}
				auto fileName = entry.path().filename().string();
				auto name = remove_number_suffix(entry.path().filename().string());
				soundBuffers[std::move(name)].emplace_back(std::move(buffer));
			}
		}
		logger.info("Sounds loaded: {}", soundBuffers.size());
	}

	void AssetMgrImpl::loadFonts() {
		for (const auto& entry : fs::directory_iterator(font_path)) {
			auto fileType = entry.path().extension();
			if (entry.is_regular_file() && (fileType == ".ttf" || fileType == ".ttc")) {
				sf::Font font;
				if (!font.openFromFile(entry.path().string())) {
					throw std::runtime_error(fmt::format("failed to load font {}", entry.path().filename().string()));
				}
				auto fontName = entry.path().filename().stem().string();
				fonts[std::move(fontName)] = font;
				logger.info("font loaded: {}", fontName);
			}
		}
		if (auto it = fonts.find(default_font); it == fonts.end()) {
			throw std::runtime_error(fmt::format("Failed to load default font {}", default_font));
		}

		logger.info("Fonts loaded: {}", fonts.size());
	}

	void AssetMgrImpl::loadTextures() {
		struct loadInfo {
			flx::StrMap<sf::Texture>& map;
			std::string path;
			std::string prefix{};
		};

		std::initializer_list<loadInfo> infos = {
			{spellTextures, spell_gfx_path},
			{UITextures, "./resources/gfx/ui"},
			{wandTextures, "./resources/gfx/wands/noita", "noita_"},
			{spriteTextures, "./resources/gfx/sprites"}};

		for (auto& info : infos) {
			logger.info("loading textures in {}", info.path);
			std::vector<std::pair<std::string, fs::path>> collected;
			for (auto& entry : fs::recursive_directory_iterator(info.path)) {
				auto fileType = entry.path().extension();
				if (entry.is_regular_file() && (fileType == ".jpg" || fileType == ".png")) {
					auto name = info.prefix + entry.path().filename().stem().string();
					collected.emplace_back(name, entry.path());
				}
			}
			for (auto& item : collected) {
				initTexture(info.map, item.first, item.second);
			}
		}
		logger.info("textures loaded: {}", spellTextures.size() + UITextures.size() + wandTextures.size());
	}

	void AssetMgr::init() {
		inst();
	}
} // namespace flx::app