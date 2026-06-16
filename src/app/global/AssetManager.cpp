// #pragma warning(disable : 5105)
// #include "AssetManager.h"
//
// #include "DataManager.h"
// #include "Loader.h"
// #include "src/utils/Container/Map.h"
// #include "src/utils/Container/Vector.h"
// #include "src/utils/Logging/Logger.h"
// #include "src/utils/Macro.h"
// #include "src/utils/Singleton.h"
//
// #include <SFML/Audio.hpp>
// #include <SFML/Graphics.hpp>
// #include <filesystem>
// #include <regex>
// #include <src/utils/Random.h>
//
// namespace flx::app {
// 	namespace fs = std::filesystem;
//
// 	auto logger = Logger::makeAsync("AssetManager");
//
// 	namespace {
// 		FLX_DEF_SINGLETON(Impl) {
// 		public:
// 			FLX_CONSTEXPR auto resource_path = "./resources";
// 			FLX_CONSTEXPR auto font_path = "./resources/fonts";
// 			FLX_CONSTEXPR auto sound_path = "./resources/sounds";
// 			FLX_CONSTEXPR auto default_font = "msyh";
// 			FLX_CONSTEXPR auto spell_gfx_path = "./resources/gfx/spells";			  // 32*32
// 			FLX_CONSTEXPR auto spell_noita_gfx_path = "./resources/gfx/spells/noita"; // 16*16
// 			FLX_CONSTEXPR auto spell_gfx_default = "default";
// 			FLX_CONSTEXPR Array<std::string_view, 2> texture_ext = {".png", ".jpg"};
//
// 			StrMap<sf::Font> fonts;
//
// 			StrMap<Vector<Unique<sf::SoundBuffer>>> soundBuffers;
// 			Vector<Unique<sf::Sound>> sounds;
//
// 			StrMap<TextureDef> textures;
// 			// input: aaa_bbb_c_N.wav, output: aaa_bbb_c
// 			static std::string remove_number_suffix(const std::string& filename) {
// 				const std::regex pattern(R"((.*)_\d+\.wav$)");
// 				std::smatch match;
//
// 				if (std::regex_match(filename, match, pattern)) {
// 					return match[1];
// 				}
// 				return filename;
// 			}
//
// 			void loadTextures() {
// 				auto onFile = [this](const fs::path& file) {
// 					const auto ext = file.extension();
// 					if (!texture_ext.view().contains(ext)) {
// 						return;
// 					}
// 					// auto& def = table[name];
//
// 					const auto entry = Loader::makeEntry(file, Loader::texture_path);
// 					if (textures.contains(entry)) {
// 						logger.warn("Skipping duplicated texture entry: {} with path {}", entry, file.string());
// 						return;
// 					}
//
// 					TextureDef def;
// 					if (!def.texture.loadFromFile(file)) {
// 						logger.warn("Failed to load texture {}", file.string());
// 						return;
// 					}
//
// 					if (const auto data = DataMgr::getTextureData(entry)) {
// 						if (data->centerAligned) {
// 							def.origin = static_cast<vec2>(def.texture.getSize()) / 2.f;
// 						}
// 						def.scale = data->scale;
// 						def.texture.setSmooth(data->smooth);
// 					} else {
// 						def.texture.setSmooth(false);
// 					}
//
// 					textures[entry] = std::move(def);
// 				};
//
// 				Loader::traverseFolder(Loader::texture_path, onFile);
// 				logger.info("Textures loaded: {}", textures.size());
// 			}
//
// 			void loadFonts() {
// 				for (const auto& entry : fs::directory_iterator(font_path)) {
// 					auto fileType = entry.path().extension();
// 					if (entry.is_regular_file() && (fileType == ".ttf" || fileType == ".ttc")) {
// 						sf::Font font;
// 						if (!font.openFromFile(entry.path().string())) {
// 							throw std::runtime_error(fmt::format("failed to load font {}", entry.path().filename().string()));
// 						}
// 						auto fontName = entry.path().filename().stem().string();
// 						fonts[std::move(fontName)] = font;
// 						logger.info("font loaded: {}", fontName);
// 					}
// 				}
// 				if (!fonts.contains(default_font)) {
// 					throw std::runtime_error(fmt::format("Failed to load default font {}", default_font));
// 				}
//
// 				logger.info("Fonts loaded: {}", fonts.size());
// 			}
// 			void loadSounds() {
// 				for (const auto& entry : fs::recursive_directory_iterator(sound_path)) {
// 					auto extension = entry.path().extension();
// 					if (entry.is_regular_file() && extension == ".wav") {
// 						auto buffer = std::make_unique<sf::SoundBuffer>();
// 						if (!buffer->loadFromFile(entry.path().string())) {
// 							auto err = std::format("Failed to load sound: {}", entry.path().string());
// 							throw std::runtime_error(err);
// 						}
// 						auto fileName = entry.path().filename().string();
// 						auto name = remove_number_suffix(entry.path().filename().string());
// 						soundBuffers[std::move(name)].emplace_back(std::move(buffer));
// 					}
// 				}
// 				logger.info("Sounds loaded: {}", soundBuffers.size());
// 			}
//
// 			TextureDef* loadSingleTexture(std::string_view fileEntry) {
// 				if (const auto t = textures.try_find(fileEntry)) {
// 					return t;
// 				}
//
// 				const auto file = Loader::texture_path / fs::path(fileEntry);
//
// 				if (!fs::exists(file)) {
// 					logger.warn("Texture file {} does not exist", file);
// 					return {};
// 				}
//
// 				TextureDef def;
// 				if (!def.texture.loadFromFile(file)) {
// 					logger.warn("Failed to load texture {}", file);
// 					return {};
// 				}
//
// 				if (const auto data = DataMgr::getTextureData(fileEntry)) {
// 					if (data->centerAligned) {
// 						def.origin = static_cast<vec2>(def.texture.getSize()) / 2.f;
// 					}
// 					def.scale = data->scale;
// 					def.texture.setSmooth(data->smooth);
// 				} else {
// 					def.texture.setSmooth(false);
// 				}
//
// 				auto& ret = textures[fileEntry];
// 				ret = std::move(def);
// 				return &ret;
// 			}
//
// 			Impl() {
// 				// loadTextures();
// 				loadFonts();
// 				loadSounds();
// 			}
// 		};
// 	} // namespace
//
// 	static auto& inst() {
// 		return Impl::inst();
// 	}
//
// 	const sf::Font& AssetMgr::getDefaultFont() {
// 		return inst().fonts.at(Impl::default_font);
// 	}
//
// 	const sf::Font& AssetMgr::getFont(std::string_view name) {
// 		if (const auto font = inst().fonts.try_find(name)) {
// 			return *font;
// 		}
// 		logger.warn("font {} does not exist", name);
// 		return getDefaultFont();
// 	}
//
// 	void AssetMgr::playSound(std::string_view name, int index) {
// 		if (inst().sounds.size() >= 50) {
// 			logger.warn("Too many sounds!");
// 			return;
// 		}
//
// 		if (const auto it = inst().soundBuffers.find(name); it != inst().soundBuffers.end()) {
// 			if (index == -1) {
// 				index = flx::random.nextVal<int>(0, static_cast<int>(it->second.size()) - 1);
// 			}
//
// 			for (const auto& sound : inst().sounds) {
// 				if (sound->getStatus() != sf::Sound::Status::Playing) {
// 					sound->setBuffer(*it->second[static_cast<size_t>(index)]);
// 					sound->play();
// 					return;
// 				}
// 			}
//
// 			auto newSound = std::make_unique<sf::Sound>(*it->second[static_cast<size_t>(index)]);
// 			newSound->setVolume(50.f);
// 			newSound->play();
// 			inst().sounds.emplace_back(std::move(newSound));
// 		} else {
// 			logger.warn("Cant find sound: {}", name);
// 		}
// 	}
//
// 	const sf::Texture* AssetMgr::getTexture(std::string_view entry) {
// 		if (const auto t = inst().loadSingleTexture(entry)) {
// 			return &t->texture;
// 		}
// 		return {};
// 	}
//
// 	const TextureDef* AssetMgr::getTextureDef(std::string_view entry) {
// 		return inst().loadSingleTexture(entry);
// 	}
//
// 	// const TextureDef& AssetMgr::getSpellTexture(std::string_view name) {
// 	// 	if (const auto it = inst().spellTextures.find(name); it != inst().spellTextures.end()) {
// 	// 		return it->second;
// 	// 	}
// 	// 	logger.warn("Spell texture '{}' does not exist", name);
// 	// 	return inst().spellTextures[inst().spell_gfx_default];
// 	// }
// 	//
// 	// const TextureDef& AssetMgr::getUITexture(std::string_view name) {
// 	// 	if (const auto it = inst().UITextures.find(name); it != inst().UITextures.end()) {
// 	// 		return it->second;
// 	// 	}
// 	// 	return getSpellTexture("default");
// 	// }
// 	//
// 	// const sf::Texture& AssetMgr::getWandTexture(std::string_view name) {
// 	// 	if (const auto it = inst().wandTextures.find(name); it != inst().wandTextures.end()) {
// 	// 		return it->second;
// 	// 	}
// 	// 	return inst().wandTextures["noita_wand_0000"];
// 	// }
//
// 	void AssetMgr::init() {
// 		inst();
// 	}
// } // namespace flx::app