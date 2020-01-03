#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <angelscript.h>
#include "AngelScript/scriptbuilder/scriptbuilder.h"
#include "AngelScript/scriptstdstring/scriptstdstring.h"
#include "AngelScript/scriptarray/scriptarray.h"
#include "AngelScript/serializer/serializer.h"
#include "AngelScript/scriptfile/scriptfile.h"
#include "AngelScript/scriptfile/scriptfilesystem.h"
#include "AngelScript/scriptgrid/scriptgrid.h"
#include "if_render.h"
#include "if_math.h"
#include "if_input.h"
#include "Console.h"
#include "TextureManager.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace AngelScript;

void Print(const std::string& msg) {
	printf("%s\n", msg.c_str());
}

void MessageCallback(const AngelScript::asSMessageInfo *msg, void *param) {
	const char *type = "ERR ";
	if (msg->type == AngelScript::asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == AngelScript::asMSGTYPE_INFORMATION)
		type = "INFO";
		
	Console::ConsolePrint(msg->message);
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

asIScriptFunction* m_InitFunc;
asIScriptFunction* m_UpdateFunc;
asIScriptFunction* m_RenderFunc;
asIScriptFunction* m_ReloadFunc;

bool LoadScript(asIScriptEngine* engine, asIScriptContext* ctx, bool reload) {
	CSerializer serializer;
	asIScriptModule* module = engine->GetModule("main.as");
	if (module) {
		serializer.Store(module);
		engine->DiscardModule("main.as");
	}
	CScriptBuilder builder;
	int r = builder.StartNewModule(engine, "main.as");
	r = builder.AddSectionFromFile("script/main.as");
	r = builder.BuildModule();
	if (r < 0) {
		return false;
	}
	module = engine->GetModule("main.as");
	m_InitFunc = module->GetFunctionByDecl("void init()");
	m_UpdateFunc = module->GetFunctionByDecl("void update(float dt)");
	m_RenderFunc = module->GetFunctionByDecl("void render()");
	m_ReloadFunc = module->GetFunctionByDecl("bool reload()");
	if (reload && m_ReloadFunc) {
		serializer.Restore(module);
		r = ctx->Prepare(m_ReloadFunc);
		r = ctx->Execute();
		return ctx->GetReturnByte() > 0;
	}
	return true;
}

int main(int argc, char** argv) {

	//init angelscript engine
	asIScriptEngine* m_asEngine = asCreateScriptEngine();
	int r = m_asEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	//register interface
	RegisterScriptArray(m_asEngine, true);
	RegisterStdString(m_asEngine);
	RegisterStdStringUtils(m_asEngine);
	RegisterScriptFile(m_asEngine);
	RegisterScriptFileSystem(m_asEngine);
	RegisterScriptGrid(m_asEngine);
	m_asEngine->RegisterGlobalFunction("void print(const string &in)", AngelScript::asFUNCTION(Print), AngelScript::asCALL_CDECL);
	if_math::LoadMathInterface(m_asEngine);
	if_render::LoadRenderInterface(m_asEngine);
	if_input::LoadInputInterface(m_asEngine);

	asIScriptContext* m_asContext = m_asEngine->CreateContext();
	//Create window
	sf::RenderWindow window(sf::VideoMode(1600, 900), "AngelGame(Running)");
	bool paused = false;
	if_render::SetWindow(&window);
	Console::Init(m_asEngine, m_asContext);
	//call init
	bool loadSuccess = LoadScript(m_asEngine, m_asContext, false);
	while(!loadSuccess) {
		//Get a chance to fix code
		DebugBreak();
		loadSuccess = LoadScript(m_asEngine, m_asContext, false);
	}
	r = m_asContext->Prepare(m_InitFunc);
	r = m_asContext->Execute();
	sf::Clock clock;
	while (window.isOpen()) {
		
		//call update
		if_input::Update();
		float deltaTime = clock.getElapsedTime().asSeconds();
		clock.restart();
		if (!paused) {
			r = m_asContext->Prepare(m_UpdateFunc);
			r = m_asContext->SetArgFloat(0, deltaTime);
			r = m_asContext->Execute();
			//call render
			r = m_asContext->Prepare(m_RenderFunc);
			r = m_asContext->Execute();
			if_render::RenderSprites();
		}
		Console::Render(&window, deltaTime);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::EventType::Closed) {
				window.close();
			}
			if_input::RegisterInputEvent(event);
			Console::RegisterKeyEvent(event);
			if (event.type == sf::Event::EventType::KeyReleased) {
				if (event.key.code == sf::Keyboard::F5) {
					bool success = LoadScript(m_asEngine, m_asContext, true);
					if (!success) {
						paused = true;
						window.setTitle("AngelGame(Paused)");
					}
					TextureManager::GetInstance().Reload();
				}
				if (event.key.code == sf::Keyboard::F1) {
					paused = !paused;
					if (paused) {
						window.setTitle("AngelGame(Paused)");
					} else {
						window.setTitle("AngelGame(Running)");
					}
					
				}
			}
		}
	}
	return 0;
}