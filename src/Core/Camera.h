#pragma once

#include <if_math.h>

struct Camera {
	Vec2 position;
	Vec2 size;
	float rotation;
	Rect viewport;
};