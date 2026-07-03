#include "Loader.h"

#include "src/utils/Container/Map.h"
#include "src/utils/Fon/Fon.h"
#include "src/utils/Logging/Logger.h"
#include "src/utils/Singleton.h"

namespace flx::app {
	static Logger logger = Logger::makeAsync("Loader");

	namespace fs = std::filesystem;

	// void Loader::traverseFolder(const Path& folder, const FileCallback& onFile) {
	// 	if (exists(folder / Loader::ignore_file))
	// 		return;
	//
	// 	for (auto& item : std::filesystem::directory_iterator(folder)) {
	// 		if (item.is_regular_file()) {
	// 			onFile(item.path());
	// 		} else if (item.is_directory()) {
	// 			traverseFolder(item.path(), onFile);
	// 		}
	// 	}
	// }
	//
	// std::string makeEntry(const Path& file, const Path& root, bool keepExtension) {
	// 	auto relative = fs::proximate(file, root);
	// 	if (!keepExtension) {
	// 		relative.replace_extension();
	// 	}
	// 	return relative.generic_string();
	// }

	namespace {
		FLX_DEF_SINGLETON(Impl) {
		public:
			FLX_STATIC_VAR auto resource_path = fs::path(Loader::resource_path);

			StrMap<std::string> failure;
			StrMap<Fon> fonData;
			StrMap<sf::Texture> textures;
			StrMap<sf::Font> fonts;

			template <class... Args>
			void fail(bool warn, std::string_view entry, std::string_view fmt, Args&&... args) {
				auto s = ::flx::vformat("msg: {}; entry: {}", vformat(fmt, std::forward<Args>(args)...), entry);
				if (warn) {
					logger.warn(s);
				}
				failure.emplace(entry, std::move(s));
			}

			const Fon* loadFon(std::string_view entry, bool required) {
				if (const auto j = fonData.try_find(entry)) {
					return j;
				}
				if (failure.contains(entry)) {
					// logger.warn(*f);
					return {};
				}

				const fs::path file = resource_path / entry;

				if (!fs::exists(file)) {
					fail(required, entry, "File {} does not exist", file.generic_string());
					return {};
				}

				try {
					const auto j = fonData.emplace(entry, *Fon::loadFromFile(file));
					return &j.first->second;
				} catch (const std::exception& e) {
					fail(true, entry, "Failed to load FON: ", e.what());
					return {};
				}
			}

			const sf::Texture* loadTexture(std::string_view entry) {
				if (const auto t = textures.try_find(entry)) {
					return t;
				}

				if (failure.contains(entry)) {
					return {};
				}

				const fs::path file = resource_path / entry;

				if (!fs::exists(file)) {
					fail(true, entry, "File {} does not exist", file.generic_string());
					return {};
				}

				sf::Texture texture;
				if (!texture.loadFromFile(file)) {
					fail(true, entry, "Failed to load texture", file.generic_string());
					return {};
				}

				texture.setSmooth(Loader::defaultSmooth);

				return &textures.emplace(entry, std::move(texture)).first->second;
			}

			const sf::Font* loadFont(std::string_view entry) {
				if (const auto f = fonts.try_find(entry)) {
					return f;
				}

				if (failure.contains(entry)) {
					return {};
				}

				const fs::path file = resource_path / entry;
				sf::Font font;
				if (!font.openFromFile(file)) {
					fail(true, entry, "Failed to load font");
					return {};
				}

				return &fonts.emplace(entry, std::move(font)).first->second;
			}
		};
	} // namespace

	auto& inst() {
		return Impl::inst();
	}

	Optional<const fon::Fon&> Loader::loadFonFile(std::string_view entry) {
		if (const auto ret = inst().loadFon(entry, true)) {
			return *ret;
		}
		// if (required) {
		// 	logger.error_and_throw("Failed to load FON: {}", entry);
		// }
		return {};
	}

	Optional<const sf::Texture&> Loader::loadTexture(std::string_view entry) {
		if (const auto ret = inst().loadTexture(entry)) {
			return *ret;
		}
		// if (required) {
		// 	logger.error_and_throw("Failed to load texture: {}", entry);
		// }
		return {};
	}

	Optional<const sf::Font&> Loader::loadFont(std::string_view entry) {
		if (const auto ret = inst().loadFont(entry)) {
			return *ret;
		}
		// if (required) {
		// 	logger.error_and_throw("Failed to load fo: {}", entry);
		// }
		return {};
	}
} // namespace flx::app
