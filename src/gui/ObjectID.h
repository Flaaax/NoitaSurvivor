#pragma once
#include"src/ecs/types.h"
#include"NObject.h"

template<class T>
class ObjectID :public NObject {
public:
	std::string_view ID()const override {
		return rawID;
	}

	static constexpr std::string_view rawID = myecs::types::type_name<T>();
};