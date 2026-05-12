#pragma once
#include <string_view>
#include<unordered_map>
#include"src/ecs/entity.h"
#include<functional>
#include"src/utils/Json.h"
#include"src/utils/Singleton.h"
#include <src/utils/VectorHelper.h>

struct GameCtx;

//Should init after EnumMeta
class ComponentMeta {
public:
	N_DECL_SINGLETON(ComponentMeta);
	using ComponentInitializer = std::function<void(const GameCtx& ctx, myecs::entity e)>;
	using ComponentInitializerFactory = ComponentInitializer(*)(const json& componentJson);

private:
	template<class T>
	using Map = std::unordered_map<std::string_view, T>;
	Map<ComponentInitializerFactory> componentInitializerFactories;

	ComponentMeta();
	void initGeneratedComponentInitializers();
	void initCustomComponentInitializers();

public:
	static ComponentInitializerFactory getInitializerFactory(std::string_view componentName) {
		return Util::value_or(inst().componentInitializerFactories, componentName, nullptr);
	}
};
