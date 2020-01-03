#include "Console.h"
#include <angelscript.h>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include "AngelScript/scripthelper/scripthelper.h"
using namespace AngelScript;

namespace Console {
	std::vector<std::string> m_History;
	std::vector<std::string> m_Log;
	std::string m_Buffer = "";
	std::string m_HiddenBuffer = "";
	bool m_Open = false;
	AngelScript::asIScriptContext* m_Ctx;
	AngelScript::asIScriptEngine* m_Engine;
	AngelScript::asIScriptModule* m_Module;
	int BufferSize = 8;
	sf::Font m_Font;
	bool m_JustOpened = false;
	int m_Selection;
	int m_Cursor;
	float m_BlinkTimer;
	float BlinkInterval = 0.8f;
	float BlinkEmptyTime = 0.3f;
	bool m_ScriptConsole = true;

	void AddHistoryEntry( const std::string& str ) {
		m_History.push_back(m_Buffer);

		std::ofstream historyFile;
		historyFile.open( "console.txt", std::ios::app );
		if ( historyFile.is_open() ) {
			historyFile << str << std::endl;
			historyFile.close();
		}
	}

	void LoadHistoryFromFile() {
		m_History.clear();
		std::ifstream historyFile;
		historyFile.open( "console.txt" );
		if ( historyFile.is_open() ) {
			std::string command;
			while ( std::getline( historyFile, command ) ) {
				m_History.push_back(command);
			}
			historyFile.close();
		}
	}

	bool IsConsoleOpen() {
		return m_Open;
	}

	void ConsolePrint(const std::string& str) {
		m_Log.push_back(str);
	}

	void Init(AngelScript::asIScriptEngine* engine, AngelScript::asIScriptContext* ctx) {
		m_Engine = engine;
		m_Ctx = ctx;
		m_Module = engine->GetModule("main.as"); //engine->GetModule("Console", asEGMFlags::asGM_CREATE_IF_NOT_EXISTS);
		m_Font.loadFromFile("assets/fonts/jackinput.ttf");
		m_Engine->RegisterGlobalFunction("bool IsConsoleOpen()", asFUNCTION(IsConsoleOpen), asCALL_CDECL);
		m_Engine->RegisterGlobalFunction("void ConsolePrint(const string str)", asFUNCTION(ConsolePrint), asCALL_CDECL);
		m_Selection = -1;
		m_Cursor = 0;
		m_BlinkTimer = 0;
		LoadHistoryFromFile();
	}

	void RegisterKeyEvent(const sf::Event& e) {
		if (e.type == sf::Event::EventType::KeyPressed && ( e.key.code == sf::Keyboard::Tilde || e.key.code == sf::Keyboard::BackSlash ) ) {
			if (e.key.shift) {
				m_ScriptConsole = false;
			} else {
				m_ScriptConsole = true;
			}
			Toggle();
			return;
		}

		if (!m_Open)
			return;
		if (e.type == sf::Event::EventType::TextEntered && !m_JustOpened) {
			if (e.text.unicode == 0x08) { //backspace
				if (!m_Buffer.empty() && m_Cursor != 0) {
					m_Buffer.erase(m_Buffer.begin() + (m_Cursor - 1));
					m_Cursor--;
				}
			} else if(e.text.unicode >= 0x20) { //visible characters
				m_Buffer.insert(m_Buffer.begin() + m_Cursor, e.text.unicode);
				m_Cursor++;
			}
		}
		m_JustOpened = false;
		if (e.type == sf::Event::EventType::KeyPressed) {
			if (e.key.code == sf::Keyboard::Enter) {
				if (!m_Buffer.empty()) {
					//execute
					if (m_ScriptConsole) {
						int r = AngelScript::ExecuteString(m_Engine, m_Buffer.c_str(), m_Module, m_Ctx);
						if (r < 0) {
							m_Log.push_back("Error executing: " + m_Buffer);
						} else {
							m_Log.push_back(m_Buffer);
						}
					} else {
						asIScriptFunction* func = m_Engine->GetModule("main.as")->GetFunctionByName("command");
						if (func) {
							int r = m_Ctx->Prepare(func);
							r = m_Ctx->SetArgObject(0, &m_Buffer);
							r = m_Ctx->Execute();
						}
					}
					AddHistoryEntry( m_Buffer );
					m_Buffer.clear();
					m_Cursor = 0;
				}
			}
			if (e.key.code == sf::Keyboard::Up) {
				if (m_Selection == -1) {
					m_HiddenBuffer = m_Buffer;
				}
				m_Selection = std::min((int)m_History.size() - 1, ++m_Selection);
				if (!m_History.empty()) {
					m_Buffer = m_History[m_History.size() - m_Selection - 1];
				}
				m_Cursor = m_Buffer.size();
			}
			if (e.key.code == sf::Keyboard::Down) {
				m_Selection = std::max(-1, --m_Selection);
				if (m_Selection == -1) {
					m_Buffer = m_HiddenBuffer;
				} else {
					if (!m_History.empty()) {
						m_Buffer = m_History[m_History.size() - m_Selection - 1];
					}
				}
				m_Cursor = m_Buffer.size();
			}
			if (e.key.code == sf::Keyboard::Right) {
				m_Cursor++;
				m_Cursor = std::min(m_Cursor, (int)m_Buffer.size());
			}
			if (e.key.code == sf::Keyboard::Left) {
				m_Cursor--;
				m_Cursor = std::max(m_Cursor, 0);
			}
		}
	}

	void Toggle() {
		m_Open = !m_Open;
		if (m_Open) m_JustOpened = true;
	}

	void Render(sf::RenderWindow* window, float dt) {
		if (!m_Open)
			return;
		m_BlinkTimer += dt;
		if (m_BlinkTimer > BlinkInterval + BlinkEmptyTime) {
			m_BlinkTimer = 0;
		}
		//draw log
		const int c = std::min(BufferSize, (int)m_Log.size());
		const int start = m_Log.size() - c;
		sf::Text t;
		t.setFont(m_Font);
		for (int i = 0; i < c; ++i) {
			t.setString(m_Log[start + i]);
			t.setPosition(0, i * 30);
			window->draw(t);
		}
		std::string beforeCursor, afterCursor;
		beforeCursor = m_Buffer.substr(0, m_Cursor);
		afterCursor = m_Buffer.substr(m_Cursor, m_Buffer.size() - m_Cursor);
		t.setString("> " + beforeCursor);
		t.setPosition(0, c * 30);
		window->draw(t);
		float textSize = t.getLocalBounds().width;
		std::string cursor = m_BlinkTimer > BlinkEmptyTime ? "|" : " ";
		t.setString(cursor + afterCursor);
		t.setPosition(textSize, c * 30);
		window->draw(t);
	}
}