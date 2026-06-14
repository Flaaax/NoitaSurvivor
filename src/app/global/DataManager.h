#pragma once
#include "../../utils/File/json.h"
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Map.h"
#include "src/utils/Singleton.h"

namespace flx::app {
	class DataMgr {
		FLX_DECL_SINGLETON(DataMgr);
		DataMgr();

		struct SpriteData {
			bool centerAligned = true;
			vec2 scale{1.f, 1.f};
			vec2 targetSize{};
			std::string texture;
		};

	private:
		flx::StrMap<SpriteData> spriteInfo;
		Json entityComponentData;

		void loadSpriteInfo();

	public:
		static const SpriteData* getSpriteData(std::string_view name);

		static const auto& getSpriteData() {
			return inst().spriteInfo;
		}

		static const Json& getEntityComponentData() {
			return inst().entityComponentData;
		}
	};
} // namespace flx::app