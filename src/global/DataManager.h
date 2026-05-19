#pragma once
#include "src/utils/Container/Map.h"
#include "src/utils/Json.h"
#include "src/utils/Singleton.h"
#include "src/utils/Vec2.h"

class DataMgr {
	N_DECL_SINGLETON(DataMgr);
	DataMgr();

	struct SpriteData {
		bool centerAligned = true;
		nvec2 scale{1.f, 1.f};
		std::string texture;
	};

private:
	Util::StdMap<SpriteData> spriteInfo;
	json entityComponentData;

	void loadSpriteInfo();

public:
	static const SpriteData* getSpriteData(std::string_view name);
	static const auto& getSpriteData() {
		return inst().spriteInfo;
	}
	static const json& getEntityComponentData() {
		return inst().entityComponentData;
	}
};