#pragma once
#include "../utils/File/json.h"
#include "src/ecs/entity.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Singleton.h"

#include "src/utils/Container/Map.h"
#include <functional>
#include <string_view>

namespace flx::game {
	struct GameCtx;
}

namespace flx::meta {

	// Should init after EnumMeta
	class ComponentMeta {
		FLX_DECL_SINGLETON(ComponentMeta);

		struct Field {
			std::string_view name;
			std::string_view type = "Unknown";
		};

		struct MetaInfo {
			flx::Vector<Field> fields;
		};

	public:
		using ComponentInitializer = std::function<void(const game::GameCtx& ctx, myecs::entity e)>;
		using ComponentInitializerFactory = ComponentInitializer (*)(const Json& componentJson);

	private:
		flx::StrMap<ComponentInitializerFactory> componentInitializerFactories;
		flx::StrMap<MetaInfo> componentMetaInfo;

		ComponentMeta();
		void initGeneratedComponentInitializers();
		void initCustomComponentInitializers();
		void initGeneratedMetaInfo();

	public:
		static ComponentInitializerFactory getInitializerFactory(std::string_view componentName) {
			return inst().componentInitializerFactories.value_or(componentName, {});
		}

		static MetaInfo* getMetaInfo(std::string_view name) {
			return inst().componentMetaInfo.try_find(name);
		}
	};

}
