#pragma once
#include <SFML/Graphics.hpp>
#include "if_math.h"
struct Sprite {
	sf::Texture* texture;
	Vec2 position;
	Vec2 scale;
	Rect textureRect;
	Vec2 textureSize;
	sf::Color tint;
	uint32_t layer;
	float rotation;
	Vec2 localOrigin;
	bool flipX = false;
	bool flipY = false;
	//internal engine stuff
	uint32_t id;
};