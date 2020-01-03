#pragma once

namespace AngelScript {
	class asIScriptEngine;
}

namespace sf {
	class Event;
	class Window;
}

namespace if_input {
	void LoadInputInterface(AngelScript::asIScriptEngine* engine);
	void RegisterInputEvent(sf::Event& e);
	void Update(sf::Window* window);
}