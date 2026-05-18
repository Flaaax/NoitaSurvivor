#include "DataManager.h"
#include "src/utils/Json.h"

void DataMgr::loadSpriteInfo() {
	auto filename = "resources/data/sprite/entity.json";
	json j = readJson(filename);
	for (const auto& dat : j["sprites"]) {
		const auto name = dat["name"].get<std::string>();
		SpriteData info{
			//.followPosition = json_parse_or(dat, "followPosition", false, filename),
			//.followAngle = json_parse_or(dat, "followAngle", false, filename),
			//.dynamicScale = json_parse_or(dat, "dynamicScale", false, filename),
			.centerAligned = json_parse_or(dat,"centerAligned",true, filename),
			//.rotationOffset = json_parse_or(dat, "rotationOffset", 0.0f, filename),
			//.positionOffset = json_parse_or(dat, "positionOffset", nvec2{0.0f, 0.0f}, filename),
			.scale = json_parse_or(dat, "scale", nvec2{1.0f, 1.0f}, filename),
			.texture = json_parse_or(dat, "texture", name, filename)
		};
		spriteInfo[std::move(name)] = info;
	}
}

DataMgr::DataMgr() {
	loadSpriteInfo();

	readJson(entityComponentData, "resources/data/component/entity.json");
}

const DataMgr::SpriteData* DataMgr::getSpriteData(std::string_view name) {
	if (auto it = inst().spriteInfo.find(name); it != inst().spriteInfo.end()) {
		return &it->second;
	}
	return nullptr;
}
