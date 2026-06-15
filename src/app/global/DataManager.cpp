#include "../global/DataManager.h"

#include "../../utils/File/json.h"
#include "Loader.h"

namespace flx::app {
	static Logger logger = Logger::makeAsync("DataManager");

	namespace fs = std::filesystem;

	namespace {
		struct Impl {
			FLX_DECL_SINGLETON(Impl);

			StrMap<AssetTable<DataMgr::TextureDefData>> textureDefs;

			void loadTextureData() {
				auto onFile = [this](const fs::path& path, std::string_view entry) {
					if (!json::isValidExtension(path.extension())) {
						return;
					}
					auto j = json::loadFromFile(path);
					for (const auto& jj : j) {
						const auto entry_name = json::getOr<std::string>(jj["file"], "");
						//const auto file=
					}
				};
			}

			Impl() {
			}
		};
	} // namespace

	void DataMgr::loadSpriteInfo() {
		const auto filename = "resources/data/gfx/entity.json";
		Json j = flx::json::loadFromFile(filename);
		for (const auto& dat : j) {
			const auto name = dat["file"].get<std::string>();
			const TextureDefData info{
				//.followPosition = json_parse_or(dat, "followPosition", false, filename),
				//.followAngle = json_parse_or(dat, "followAngle", false, filename),
				//.dynamicScale = json_parse_or(dat, "dynamicScale", false, filename),
				.centerAligned = json::parseOr(dat, "centerAligned", true, filename),
				//.rotationOffset = json_parse_or(dat, "rotationOffset", 0.0f, filename),
				//.positionOffset = json_parse_or(dat, "positionOffset", nvec2{0.0f, 0.0f}, filename),
				.scale = json::parseOr(dat, "scale", vec2{1.0f, 1.0f}, filename),
				.targetSize = json::parseOr(dat, "targetSize", vec2{}, filename),
				.smooth = json::parseOr(dat, "smooth", false, filename),
				.entry = json::parseOr(dat, "entry", name, filename),
			};

			textureDefs[std::move(name)] = info;
		}
	}

	void DataMgr::loadTextureData() {
	}

	DataMgr::DataMgr() {
		loadSpriteInfo();

		entityComponentData = flx::json::loadFromFile("resources/data/component/entity.json");
	}

	const DataMgr::TextureDefData* DataMgr::getTextureData(std::string_view entry, std::string_view name) {
		if (entry != "entity") {
			return {};
		}
		return inst().textureDefs.try_find(name);
	}
} // namespace flx::app