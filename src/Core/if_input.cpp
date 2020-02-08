#include "if_input.h"
#include <angelscript.h>
#include <SFML/Window.hpp>
#include "if_math.h"
#include "Camera.h"

using namespace AngelScript;

struct InputState {
	bool keyState[sf::Keyboard::KeyCount];
	bool lastKeyState[sf::Keyboard::KeyCount];
	bool mouseButtonState[sf::Mouse::ButtonCount];
	bool lastMouseButtonState[sf::Mouse::ButtonCount];
	sf::Window* window;
};

static InputState g_State;

bool IsKeyDown(sf::Keyboard::Key key) {
	return g_State.keyState[key];
}

bool IsKeyPushed(sf::Keyboard::Key key) {
	return g_State.keyState[key] && !g_State.lastKeyState[key];
}

bool IsKeyReleased(sf::Keyboard::Key key) {
	return !g_State.keyState[key] && g_State.lastKeyState[key];
}

bool IsMouseButtonDown(sf::Mouse::Button button) {
	return g_State.mouseButtonState[button];
}

bool IsMouseButtonPushed(sf::Mouse::Button button) {
	bool ret = g_State.mouseButtonState[button] && !g_State.lastMouseButtonState[button];
	return ret;
}

bool IsMouseButtonReleased(sf::Mouse::Button button) {
	return !g_State.mouseButtonState[button] && g_State.lastMouseButtonState[button];
}

Vec2 MousePos() {
	sf::Vector2i p = sf::Mouse::getPosition(*g_State.window);
	return Vec2(p.x, p.y);
}

namespace if_input {
	void LoadInputInterface(AngelScript::asIScriptEngine* engine) {
		engine->RegisterEnum("Key");
		engine->RegisterEnumValue("Key", "A", sf::Keyboard::A);
		engine->RegisterEnumValue("Key", "B", sf::Keyboard::B);
		engine->RegisterEnumValue("Key", "C", sf::Keyboard::C);
		engine->RegisterEnumValue("Key", "D", sf::Keyboard::D);
		engine->RegisterEnumValue("Key", "E", sf::Keyboard::E);
		engine->RegisterEnumValue("Key", "F", sf::Keyboard::F);
		engine->RegisterEnumValue("Key", "G", sf::Keyboard::G);
		engine->RegisterEnumValue("Key", "H", sf::Keyboard::H);
		engine->RegisterEnumValue("Key", "I", sf::Keyboard::I);
		engine->RegisterEnumValue("Key", "J", sf::Keyboard::J);
		engine->RegisterEnumValue("Key", "K", sf::Keyboard::K);
		engine->RegisterEnumValue("Key", "L", sf::Keyboard::L);
		engine->RegisterEnumValue("Key", "M", sf::Keyboard::M);
		engine->RegisterEnumValue("Key", "N", sf::Keyboard::N);
		engine->RegisterEnumValue("Key", "O", sf::Keyboard::O);
		engine->RegisterEnumValue("Key", "P", sf::Keyboard::P);
		engine->RegisterEnumValue("Key", "Q", sf::Keyboard::Q);
		engine->RegisterEnumValue("Key", "R", sf::Keyboard::R);
		engine->RegisterEnumValue("Key", "S", sf::Keyboard::S);
		engine->RegisterEnumValue("Key", "T", sf::Keyboard::T);
		engine->RegisterEnumValue("Key", "U", sf::Keyboard::U);
		engine->RegisterEnumValue("Key", "V", sf::Keyboard::V);
		engine->RegisterEnumValue("Key", "W", sf::Keyboard::W);
		engine->RegisterEnumValue("Key", "X", sf::Keyboard::X);
		engine->RegisterEnumValue("Key", "Y", sf::Keyboard::Y);
		engine->RegisterEnumValue("Key", "Z", sf::Keyboard::Z);
		engine->RegisterEnumValue("Key", "Num0", sf::Keyboard::Num0);
		engine->RegisterEnumValue("Key", "Num1", sf::Keyboard::Num1);
		engine->RegisterEnumValue("Key", "Num2", sf::Keyboard::Num2);
		engine->RegisterEnumValue("Key", "Num3", sf::Keyboard::Num3);
		engine->RegisterEnumValue("Key", "Num4", sf::Keyboard::Num4);
		engine->RegisterEnumValue("Key", "Num5", sf::Keyboard::Num5);
		engine->RegisterEnumValue("Key", "Num6", sf::Keyboard::Num6);
		engine->RegisterEnumValue("Key", "Num7", sf::Keyboard::Num7);
		engine->RegisterEnumValue("Key", "Num8", sf::Keyboard::Num8);
		engine->RegisterEnumValue("Key", "Num9", sf::Keyboard::Num9);
		engine->RegisterEnumValue("Key", "Escape", sf::Keyboard::Escape);
		engine->RegisterEnumValue("Key", "LControl", sf::Keyboard::LControl);
		engine->RegisterEnumValue("Key", "LShift", sf::Keyboard::LShift);
		engine->RegisterEnumValue("Key", "LAlt", sf::Keyboard::LAlt);
		engine->RegisterEnumValue("Key", "LSystem", sf::Keyboard::LSystem);
		engine->RegisterEnumValue("Key", "RControl", sf::Keyboard::RControl);
		engine->RegisterEnumValue("Key", "RShift", sf::Keyboard::RShift);
		engine->RegisterEnumValue("Key", "RAlt", sf::Keyboard::RAlt);
		engine->RegisterEnumValue("Key", "RSystem", sf::Keyboard::RSystem);
		engine->RegisterEnumValue("Key", "Menu", sf::Keyboard::Menu);
		engine->RegisterEnumValue("Key", "LBracket", sf::Keyboard::LBracket);
		engine->RegisterEnumValue("Key", "RBracket", sf::Keyboard::RBracket);
		engine->RegisterEnumValue("Key", "Semicolon", sf::Keyboard::Semicolon);
		engine->RegisterEnumValue("Key", "Comma", sf::Keyboard::Comma);
		engine->RegisterEnumValue("Key", "Period", sf::Keyboard::Period);
		engine->RegisterEnumValue("Key", "Quote", sf::Keyboard::Quote);
		engine->RegisterEnumValue("Key", "Slash", sf::Keyboard::Slash);
		engine->RegisterEnumValue("Key", "Backslash", sf::Keyboard::Backslash);
		engine->RegisterEnumValue("Key", "Tilde", sf::Keyboard::Tilde);
		engine->RegisterEnumValue("Key", "Equal", sf::Keyboard::Equal);
		engine->RegisterEnumValue("Key", "Hyphen", sf::Keyboard::Hyphen);
		engine->RegisterEnumValue("Key", "Space", sf::Keyboard::Space);
		engine->RegisterEnumValue("Key", "Enter", sf::Keyboard::Enter);
		engine->RegisterEnumValue("Key", "Backspace", sf::Keyboard::Backspace);
		engine->RegisterEnumValue("Key", "Tab", sf::Keyboard::Tab);
		engine->RegisterEnumValue("Key", "PageUp", sf::Keyboard::PageUp);
		engine->RegisterEnumValue("Key", "PageDown", sf::Keyboard::PageDown);
		engine->RegisterEnumValue("Key", "End", sf::Keyboard::End);
		engine->RegisterEnumValue("Key", "Home", sf::Keyboard::Home);
		engine->RegisterEnumValue("Key", "Insert", sf::Keyboard::Insert);
		engine->RegisterEnumValue("Key", "Delete", sf::Keyboard::Delete);
		engine->RegisterEnumValue("Key", "Add", sf::Keyboard::Add);
		engine->RegisterEnumValue("Key", "Subtract", sf::Keyboard::Subtract);
		engine->RegisterEnumValue("Key", "Multiply", sf::Keyboard::Multiply);
		engine->RegisterEnumValue("Key", "Divide", sf::Keyboard::Divide);
		engine->RegisterEnumValue("Key", "Left", sf::Keyboard::Left);
		engine->RegisterEnumValue("Key", "Right", sf::Keyboard::Right);
		engine->RegisterEnumValue("Key", "Up", sf::Keyboard::Up);
		engine->RegisterEnumValue("Key", "Down", sf::Keyboard::Down);
		engine->RegisterEnumValue("Key", "Numpad0", sf::Keyboard::Numpad0);
		engine->RegisterEnumValue("Key", "Numpad1", sf::Keyboard::Numpad1);
		engine->RegisterEnumValue("Key", "Numpad2", sf::Keyboard::Numpad2);
		engine->RegisterEnumValue("Key", "Numpad3", sf::Keyboard::Numpad3);
		engine->RegisterEnumValue("Key", "Numpad4", sf::Keyboard::Numpad4);
		engine->RegisterEnumValue("Key", "Numpad5", sf::Keyboard::Numpad5);
		engine->RegisterEnumValue("Key", "Numpad6", sf::Keyboard::Numpad6);
		engine->RegisterEnumValue("Key", "Numpad7", sf::Keyboard::Numpad7);
		engine->RegisterEnumValue("Key", "Numpad8", sf::Keyboard::Numpad8);
		engine->RegisterEnumValue("Key", "Numpad9", sf::Keyboard::Numpad9);
		engine->RegisterEnumValue("Key", "F1", sf::Keyboard::F1);
		engine->RegisterEnumValue("Key", "F2", sf::Keyboard::F2);
		engine->RegisterEnumValue("Key", "F3", sf::Keyboard::F3);
		engine->RegisterEnumValue("Key", "F4", sf::Keyboard::F4);
		engine->RegisterEnumValue("Key", "F5", sf::Keyboard::F5);
		engine->RegisterEnumValue("Key", "F6", sf::Keyboard::F6);
		engine->RegisterEnumValue("Key", "F7", sf::Keyboard::F7);
		engine->RegisterEnumValue("Key", "F8", sf::Keyboard::F8);
		engine->RegisterEnumValue("Key", "F9", sf::Keyboard::F9);
		engine->RegisterEnumValue("Key", "F10", sf::Keyboard::F10);
		engine->RegisterEnumValue("Key", "F11", sf::Keyboard::F11);
		engine->RegisterEnumValue("Key", "F12", sf::Keyboard::F12);
		engine->RegisterEnumValue("Key", "Pause", sf::Keyboard::Pause);

		engine->RegisterEnum("MouseButton");
		engine->RegisterEnumValue("MouseButton", "Left", sf::Mouse::Button::Left);
		engine->RegisterEnumValue("MouseButton", "Right", sf::Mouse::Button::Right);
		engine->RegisterEnumValue("MouseButton", "Middle", sf::Mouse::Button::Middle);
		engine->RegisterEnumValue("MouseButton", "Forward", sf::Mouse::Button::XButton1);
		engine->RegisterEnumValue("MouseButton", "Backward", sf::Mouse::Button::XButton2);

		engine->RegisterGlobalFunction("bool IsKeyDown(Key k)", asFUNCTION(IsKeyDown), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool IsKeyPushed(Key k)", asFUNCTION(IsKeyPushed), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool IsKeyReleased(Key k)", asFUNCTION(IsKeyReleased), asCALL_CDECL);

		engine->RegisterGlobalFunction("bool IsMouseButtonDown(MouseButton mb)", asFUNCTION(IsMouseButtonDown), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool IsMouseButtonPushed(MouseButton mb)", asFUNCTION(IsMouseButtonPushed), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool IsMouseButtonReleased(MouseButton mb)", asFUNCTION(IsMouseButtonReleased), asCALL_CDECL);

		engine->RegisterGlobalFunction("Vec2 MousePos()", asFUNCTION(MousePos), asCALL_CDECL);

		memset(&g_State.keyState, 0x0, sizeof(bool) * sf::Keyboard::KeyCount);
		memset(&g_State.mouseButtonState, 0x0, sizeof(bool)* sf::Mouse::Button::ButtonCount);
	}

	void RegisterInputEvent(sf::Event& e) {
		if (e.type == sf::Event::EventType::KeyPressed) {
			g_State.keyState[e.key.code] = true;
		}
		else if(e.type == sf::Event::EventType::KeyReleased) {
			g_State.keyState[e.key.code] = false;
		}
		else if (e.type == sf::Event::EventType::MouseButtonPressed) {
			g_State.mouseButtonState[e.mouseButton.button] = true;
		}
		else if (e.type == sf::Event::EventType::MouseButtonReleased) {
			g_State.mouseButtonState[e.mouseButton.button] = false;
		}
	}

	void Update(sf::Window* window) {
		g_State.window = window;
		memcpy(&g_State.lastKeyState, &g_State.keyState, sizeof(bool) * sf::Keyboard::KeyCount);
		memcpy(&g_State.lastMouseButtonState, &g_State.mouseButtonState, sizeof(bool) * sf::Mouse::Button::ButtonCount);
	}
}