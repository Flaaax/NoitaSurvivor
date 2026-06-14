#include "../global/DataManager.h"
#include "../../utils/File/json.h"

namespace flx::app {
	void DataMgr::loadSpriteInfo() {
		const auto filename = "resources/data/sprite/entity.json";
		Json j = flx::json::loadFromFile(filename);
		for (const auto& dat : j["sprites"]) {
			const auto name = dat["name"].get<std::string>();
			const SpriteData info{
				//.followPosition = json_parse_or(dat, "followPosition", false, filename),
				//.followAngle = json_parse_or(dat, "followAngle", false, filename),
				//.dynamicScale = json_parse_or(dat, "dynamicScale", false, filename),
				.centerAligned = json::parseOr(dat, "centerAligned", true, filename),
				//.rotationOffset = json_parse_or(dat, "rotationOffset", 0.0f, filename),
				//.positionOffset = json_parse_or(dat, "positionOffset", nvec2{0.0f, 0.0f}, filename),
				.scale = json::parseOr(dat, "scale", vec2{1.0f, 1.0f}, filename),
				.targetSize = json::parseOr(dat, "targetSize", vec2{}, filename),
				.texture = json::parseOr(dat, "texture", name, filename)};

			spriteInfo[std::move(name)] = info;
		}
	}

	DataMgr::DataMgr() {
		loadSpriteInfo();

		entityComponentData = flx::json::loadFromFile("resources/data/component/entity.json");
	}

	const DataMgr::SpriteData* DataMgr::getSpriteData(std::string_view name) {
		if (const auto it = inst().spriteInfo.find(name); it != inst().spriteInfo.end()) {
			return &it->second;
		}
		return nullptr;
	}
} // namespace flx::app