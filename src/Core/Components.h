#pragma once
#include "sprite.h"
#include "if_math.h"


enum ComponentType {
	TRANSFORM = 0x1,
	SPRITE = 0x2,
	PHYSICS = 0x4
};

struct TransformComponent {
	Vec2 position;
	Vec2 scale;
	Vec2 localOrigin;
	float rotation;
};

struct SpriteComponent {
	sf::Texture* texture;
	Rect textureRect;
	Vec2 textureSize;
	sf::Color tint;
	uint32_t layer;
};

struct PhysicsComponent {
	//todo add shit
};

namespace AngelScript {
	class asIScriptEngine;
};
namespace Components {
	void LoadComponentInterface(AngelScript::asIScriptEngine* engine);
}