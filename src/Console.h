#pragma once

namespace AngelScript {
	class asIScriptContext;
	class asIScriptEngine;
}

namespace sf {
	class RenderWindow;
	class Event;
}

namespace Console {
	void Init(AngelScript::asIScriptEngine* engine, AngelScript::asIScriptContext* ctx);
	void RegisterKeyEvent(const sf::Event& e);
	void Toggle();
	void Render(sf::RenderWindow* window, float dt);
}