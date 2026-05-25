#pragma once
#include "../utils/File/Json.h"
#include "../utils/Vec2/Vec2.h"
#include "src/utils/Container/Map.h"
#include "src/utils/Singleton.h"

class DataMgr {
	N_DECL_SINGLETON(DataMgr);
	DataMgr();

	struct SpriteData {
		bool centerAligned = true;
		nvec2 scale{1.f, 1.f};
		std::string texture;
	};

private:
	Util::StrMap<SpriteData> spriteInfo;
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