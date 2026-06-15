#pragma once
#include "../../utils/File/json.h"
#include "../../utils/Vec2/Vec2.h"
#include "src/utils/Container/Map.h"
#include "src/utils/Macro.h"
#include "src/utils/Singleton.h"

namespace flx::app {
	class DataMgr {
		FLX_DECL_SINGLETON(DataMgr);

	private:
		DataMgr();

		Json entityComponentData;

		void loadSpriteInfo();
		void loadTextureData();

	public:
		FLX_CONSTEXPR std::string_view config_file = "config.jsonc";
		FLX_CONSTEXPR std::string_view texturePath = "./resources/gfx/";

		struct TextureDefData {
			bool centerAligned = true;
			vec2 scale{1.f, 1.f};
			vec2 targetSize{};
			bool smooth = false;
			std::string entry;
		};

		static const TextureDefData* getTextureData(std::string_view entry, std::string_view name);

		static const Json& getEntityComponentData() {
			return inst().entityComponentData;
		}
	};
} // namespace flx::app