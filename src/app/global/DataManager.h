#pragma once
// #include "../../utils/Vec2/Vec2.h"
// #include "src/utils/Container/Map.h"
// #include "src/utils/Macro.h"
//
// namespace flx::json {
// 	class Json;
// }
//
// namespace flx::app {
// 	class DataMgr {
// 	public:
// 		FLX_CONSTEXPR std::string_view config_file = "config.jsonc";
// 		FLX_CONSTEXPR std::string_view texturePath = "./resources/gfx/";
//
// 		struct TextureDefData {
// 			bool centerAligned = false;
// 			vec2 scale{1.f, 1.f};
// 			vec2 targetSize{};
// 			bool smooth = false;
// 		};
//
// 		//static const TextureDefData* getTextureData(std::string_view entry);
// 		//static const json::Json& getJsonData(std::string_view entry);
// 		static void load();
// 	};
// } // namespace flx::app