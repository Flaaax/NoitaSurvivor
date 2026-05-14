#pragma once
#include <box2d/types.h>

//warning: If the world step is multithreaded, all of those callbacks should be thread-safe!	Currently: single threaded
class PhysicalContactCallbacks {
public:
	static bool FilterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context);
	static bool PresolveCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);
};
