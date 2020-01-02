#pragma once

namespace AngelScript {
	class asIScriptEngine;
}

struct Vec2 {
	Vec2() {}
	Vec2(float v) { x = y = v; }
	Vec2(float x, float y) { this->x = x; this->y = y; }
	void operator-=(const Vec2 other){
		this->x -= other.x;
		this->y -= other.y;
	}
	Vec2 operator-(const Vec2 other) const {
		Vec2 r;
		r.x = this->x - other.x;
		r.y = this->y - other.y;
		return r;
	}
	void operator+=(const Vec2 other) {
		this->x += other.x;
		this->y += other.y;
	}
	Vec2 operator+(const Vec2 other) const {
		Vec2 r;
		r.x = this->x + other.x;
		r.y = this->y + other.y;
		return r;
	}
	float x;
	float y;
};

struct Rect {
	float x;
	float y;
	float w;
	float h;
};

namespace if_math {
	void LoadMathInterface(AngelScript::asIScriptEngine* engine);
}