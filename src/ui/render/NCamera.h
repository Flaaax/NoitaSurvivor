#pragma once
#include<SFML/System/Vector2.hpp>
#include"src/utils/Math.h"


class NCamera {
public:
	nvec2 renderPos = { 0,0 };

	nvec2 offeset(const nvec2& v) const {
		return v - renderPos;
	}
};

