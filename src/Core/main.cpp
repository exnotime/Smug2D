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
#include "AngelScript/scripthandle/scripthandle.h"
#include "AngelScript/scripthelper/scripthelper.h"
#include "AngelScript/scriptdictionary/scriptdictionary.h"
#include "if_render.h"
#include "if_math.h"
#include "if_input.h"
#include "Console.h"
#include "Components.h"
#include "if_Entity.h"
#include "PhysicsManager.h"
#include "TextureManager.h"
#include "SpriteAnimation.h"
#include "if_animation.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "AngelScript/AngelScriptExporter.h"
#include "AngelScript/AngelScriptDebugger.h"



#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

using namespace AngelScript;

void Print(const std::string& msg) {
	printf("%s\n", msg.c_str());
}

void Break() {
	DebugBreak();
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

void ExceptionCallback() {

}

struct ProgramArguments {
	bool WaitForDebugger = false;
};

ProgramArguments ParseProgramArgs(int argc, char** argv) {
	ProgramArguments args;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--wait-for-debugger") == 0) {
			args.WaitForDebugger = true;
		}
	}
	return args;
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

bool TestCompileScript(asIScriptEngine* engine) {
	CScriptBuilder builder;
	int r = builder.StartNewModule(engine, "TestMain.as");
	r = builder.AddSectionFromFile("script/main.as");
	r = builder.BuildModule();
	if (r < 0) {
		return false;
	}
	return true;
}

int main(int argc, char** argv) {
	ProgramArguments args = ParseProgramArgs(argc, argv);

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
	RegisterScriptHandle(m_asEngine);
	RegisterScriptDictionary(m_asEngine);
	m_asEngine->RegisterGlobalFunction("void print(const string &in)", AngelScript::asFUNCTION(Print), AngelScript::asCALL_CDECL);
	m_asEngine->RegisterGlobalFunction("void Break()", AngelScript::asFUNCTION(Break), AngelScript::asCALL_CDECL);
	m_asEngine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true); //to have '' mean a single char
	if_math::LoadMathInterface(m_asEngine);
	if_render::LoadRenderInterface(m_asEngine);
	if_input::LoadInputInterface(m_asEngine);
	if_animation::LoadAnimationInterface(m_asEngine);
	if_entity::LoadEntityInterface(m_asEngine);
	Components::LoadComponentInterface(m_asEngine);
	srand(time(0));
	asIScriptContext* m_asContext = m_asEngine->CreateContext();
	//Create window
	sf::RenderWindow window(sf::VideoMode(1600, 900), "AngelGame(Running)");
	bool paused = false;
	if_render::SetWindow(&window);
	Console::Init(m_asEngine, m_asContext);
	ImGui::SFML::Init(window);
	// Create Physics
	PhysicsManager::GetInstance().Init();
	//Export interface to file
	m_asEngine->SetDefaultNamespace("Smug2D");
	AngelScriptExporter::ExportEngineAsJSON("interface.json", m_asEngine);
	
	ASDebugger debugger;
	debugger.Start(9002);
	debugger.SetContext(m_asContext);

	sf::Clock debugTimer;
	const float timeout = 10.0f;
	if (args.WaitForDebugger) {
		while (debugTimer.getElapsedTime().asSeconds() < timeout) {
			sf::Event event;
			while (window.pollEvent(event)) {};
			window.clear();
			window.display();
			debugger.Update();
			//wait until we have a connection and have received all pre-set breakpoints
			if (debugger.IsConnected() && debugger.IsSetup()) {
				break;
			}
		}
	}

	//call init
	bool loadSuccess = LoadScript(m_asEngine, m_asContext, false);
	while(!loadSuccess) {
		//Get a chance to fix code
		DebugBreak();
		loadSuccess = LoadScript(m_asEngine, m_asContext, false);
	}
	r = m_asContext->Prepare(m_InitFunc);
	r = m_asContext->Execute();
	//if we hit a breakpoint during init we need to wait for the user to finish
	while (debugger.IsSuspended()){
		sf::Event event;
		while (window.pollEvent(event)) {};
		window.clear();
		window.display();
		debugger.Update();
	}
	if (r == asEXECUTION_SUSPENDED) {
		m_asContext->Execute();
	}
	if (r == asEXECUTION_EXCEPTION) {
		printf("Exception! %s\n", m_asContext->GetExceptionString());
		int column;
		const char* fileName;
		int line = m_asContext->GetExceptionLineNumber(&column, &fileName);
		printf("File:%s(%d)\n",fileName, line);
		DebugBreak();
	}

	bool imguiOpen = false;
	sf::Clock clock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
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
					}
					else {
						window.setTitle("AngelGame(Running)");
					}
				}
				if (event.key.code == sf::Keyboard::Tab) {
					imguiOpen = !imguiOpen;
				}
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
		}

		//call update
		sf::Time deltaTime = clock.restart();
		if (imguiOpen) {
			ImGui::SFML::Update(window, deltaTime);
		} else {
			ImGui::NewFrame();
		}
		float dt = deltaTime.asSeconds();
		
		SpriteAnimation::Update(dt);// update all animation components
		if (!paused && !debugger.IsSuspended()) {
			r = m_asContext->Prepare(m_UpdateFunc);
			r = m_asContext->SetArgFloat(0, dt);
			r = m_asContext->Execute();
		}
		window.clear();
		if (!paused && !debugger.IsSuspended()) {
			r = m_asContext->Prepare(m_RenderFunc);
			r = m_asContext->Execute();

			if_render::Render();
		}

		Console::Render(&window, dt);
		if (imguiOpen) {
			ImGui::SFML::Render();
			window.resetGLStates();
		}
		else {
			ImGui::EndFrame();
		}
		debugger.Update();
		if_input::Update(&window);
		window.display();
		
	}
	return 0;
}