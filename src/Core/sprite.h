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
	//internal engine stuff
	uint32_t id;
};