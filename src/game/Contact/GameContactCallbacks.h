#pragma once
#include <box2d/types.h>

class GameContactCallbacks {
public:
	static bool FilterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context);
};
