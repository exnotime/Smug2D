#pragma once
#include <SFML/Graphics.hpp>
#include "if_math.h"
struct Sprite {
	sf::Texture* texture;
	Vec2 postion;
	Vec2 scale;
	Rect textureRect;
	Vec2 textureSize;
	sf::Color tint;
};