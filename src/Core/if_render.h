#pragma once
#include <if_math.h>

namespace AngelScript {
	class asIScriptEngine;
}

namespace sf {
	class RenderWindow;
	class Texture;
}

namespace if_render {
	void LoadRenderInterface(AngelScript::asIScriptEngine* engine);
	void SetWindow(sf::RenderWindow* window);
	void Render();
}