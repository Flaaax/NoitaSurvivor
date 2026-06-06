#pragma once
#include "../utils/File/Json.h"
#include "src/ecs/entity.h"
#include "src/utils/Container/Vector.h"
#include "src/utils/Singleton.h"

#include "src/utils/Container/Map.h"
#include <functional>
#include <string_view>

struct GameCtx;

// Should init after EnumMeta
class ComponentMeta {
	N_DECL_SINGLETON(ComponentMeta);

	struct Field {
		std::string_view name;
		std::string_view type = "Unknown";
	};

	struct MetaInfo {
		Util::Vector<Field> fields;
	};

public:
	using ComponentInitializer = std::function<void(const GameCtx& ctx, myecs::entity e)>;
	using ComponentInitializerFactory = ComponentInitializer (*)(const json& componentJson);

private:
	Util::StrMap<ComponentInitializerFactory> componentInitializerFactories;
	Util::StrMap<MetaInfo> componentMetaInfo;

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
