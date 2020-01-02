#pragma once

namespace AngelScript {
	class asIScriptEngine;
}

namespace sf {
	class RenderWindow;
}

namespace if_render {
	void LoadRenderInterface(AngelScript::asIScriptEngine* engine);
	void SetWindow(sf::RenderWindow* window);
	void RenderSprites();
}