#pragma once

#include <if_math.h>
#include <AngelScript/RefObject.h>
struct Camera {
	Vec2 position;
	Vec2 size;
	float rotation;
	Rect viewport;
	REF_COUNTER;
};