#pragma once

namespace AngelScript {
	class asIScriptEngine;
}

namespace sf {
	class Event;
}

namespace if_input {
	void LoadInputInterface(AngelScript::asIScriptEngine* engine);
	void RegisterInputEvent(sf::Event& e);
	void Update();
}