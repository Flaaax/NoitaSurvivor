// #include "../global/DataManager.h"
//
// #include "../../utils/File/Json.h"
// #include "../../utils/File/JsonExt.h"
// #include "Loader.h"
// #include "src/utils/Logging/Logger.h"
// #include "src/utils/Singleton.h"
//
// namespace flx::app {
// 	static Logger logger = Logger::makeAsync("DataManager");
//
// 	namespace fs = std::filesystem;
//
// 	namespace {
// 		class Impl {
// 			FLX_DECL_SINGLETON(Impl);
//
// 		public:
// 			StrMap<DataMgr::TextureDefData> textureDefs;
// 			Json entityComponentData;
//
// 			void loadTextureData() {
// 				auto onFile = [this](const fs::path& path) {
// 					const auto ext = path.extension().string();
// 					if (ext != ".jsonc" && ext != ".json") {
// 						return;
// 					}
// 					const auto folder = path.parent_path();
// 					const auto j = Json::loadFromFile(path);
// 					for (const auto& jj : j) {
// 						const auto textureFile = folder / jj.get<fs::path>("file");
// 						const auto entry = Loader::makeEntry(textureFile, DataMgr::texturePath);
// 						if (textureDefs.contains(entry)) {
// 							logger.warn("Skipping dulplicated texture entry: {}", entry);
// 							continue;
// 						}
//
// 						DataMgr::TextureDefData data;
// 						data.scale = json::getIfVec2(jj, "scale").value_or({1.f, 1.f});
// 						data.smooth = jj.getOr("smooth", false);
// 						data.centerAligned = jj.getOr("centerAligned", false);
// 						textureDefs[entry] = std::move(data);
// 					}
// 				};
//
// 				Loader::traverseFolder(DataMgr::texturePath, onFile);
// 			}
//
// 			Impl() {
// 				loadTextureData();
// 				entityComponentData = Json::loadFromFile("resources/data/component/entity.json");
// 			}
// 		};
// 	} // namespace
//
// 	static Impl& inst() {
// 		return Impl::inst();
// 	}
//
// 	// void DataMgr::loadSpriteInfo() {
// 	// 	const auto filename = "resources/data/gfx/entity.json";
// 	// 	Json j = flx::json::loadFromFile(filename);
// 	// 	for (const auto& dat : j) {
// 	// 		const auto name = dat["file"].get<std::string>();
// 	// 		const TextureDefData info{
// 	// 			//.followPosition = json_parse_or(dat, "followPosition", false, filename),
// 	// 			//.followAngle = json_parse_or(dat, "followAngle", false, filename),
// 	// 			//.dynamicScale = json_parse_or(dat, "dynamicScale", false, filename),
// 	// 			.centerAligned = json::parseOr(dat, "centerAligned", true, filename),
// 	// 			//.rotationOffset = json_parse_or(dat, "rotationOffset", 0.0f, filename),
// 	// 			//.positionOffset = json_parse_or(dat, "positionOffset", nvec2{0.0f, 0.0f}, filename),
// 	// 			.scale = json::parseOr(dat, "scale", vec2{1.0f, 1.0f}, filename),
// 	// 			.targetSize = json::parseOr(dat, "targetSize", vec2{}, filename),
// 	// 			.smooth = json::parseOr(dat, "smooth", false, filename),
// 	// 			.entry = json::parseOr(dat, "entry", name, filename),
// 	// 		};
// 	//
// 	// 		textureDefs[std::move(name)] = info;
// 	// 	}
// 	// }
//
// 	const DataMgr::TextureDefData* DataMgr::getTextureData(std::string_view entry) {
// 		return inst().textureDefs.try_find(entry);
// 	}
//
// 	const json::Json& DataMgr::getJsonData(std::string_view entry) {
// 	}
//
// 	void DataMgr::load() {
// 		inst();
// 	}
// } // namespace flx::app