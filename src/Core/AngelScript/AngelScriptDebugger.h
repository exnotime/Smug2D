#pragma once
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <thread>
#include <functional>
#include <vector>
#include <mutex>

#include <angelscript.h>
#include "AngelScript/debugger/debugger.h"
#include <meow_hash_x64_aesni.h>

class Socket {
public:
	Socket(){
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		m_Connection = NULL;
		m_ListenSocket = NULL;
		
	}
	~Socket(){
	}

	void close() {
		closesocket(m_ListenSocket);
		::shutdown(m_Connection, SD_SEND);
		closesocket(m_Connection);
	}

	void listen(int port, const char* address) {
		m_ListenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ListenSocket == INVALID_SOCKET) {
			int error = WSAGetLastError();
			printf("last error = %d\n", error);
		}
		sockaddr_in address_in;
		address_in.sin_family = AF_INET;
		address_in.sin_addr.S_un.S_addr = inet_addr(address);
		address_in.sin_port = htons(port);
		int result = ::bind(m_ListenSocket, (sockaddr*)&address_in, sizeof(address_in));

		if (result == SOCKET_ERROR) {
			int error = WSAGetLastError();
			printf("last error = %d\n", error);
		}
		if (::listen(m_ListenSocket, SOMAXCONN) != SOCKET_ERROR) {
			m_Connection = ::accept(m_ListenSocket, NULL, NULL);
			if (m_Connection == INVALID_SOCKET) {
				int error = WSAGetLastError();
				printf("last error = %d\n", error);
			}
			closesocket(m_ListenSocket);
		} else {
			int error = WSAGetLastError();
			printf("last error = %d\n", error);
		}
	}

	void send(void* buffer, int buffersize) {
		::send(m_Connection, (const char*)buffer, buffersize, 0);
	}

	int recieve(char* buffer, int bufferSize) {
		return ::recv(m_Connection, buffer, bufferSize, 0);
	}

private:
	SOCKET m_Connection;
	SOCKET m_ListenSocket;
};


void DebugLineCallback(asIScriptContext* ctx, void* dbg);

struct AsBreakpoint {
	uint32_t lineNumber;
	uint32_t id;
};

uint32_t HashBreakpoint(uint32_t line, const char* filename) {
	uint32_t bufferSize = strlen(filename) + sizeof(uint32_t) + 16;
	char* buffer = (char*)malloc(bufferSize);
	sprintf(buffer, "%s%d", filename, line);
	return MeowU32From(MeowHash(MeowDefaultSeed, bufferSize, buffer), 0);
}

void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

class ASDebugger {
public:
	ASDebugger() {};

	void Start(int port) {
		m_ServerThread = std::thread(&ASDebugger::ServerThread, this);
	}

	void Splitstring(const std::string& str, const std::string& delim, std::vector<std::string>& output) {
		std::string s = str;
		size_t pos = s.find(delim);
		if (pos == std::string::npos) {
			output.push_back(s);
		}
		while (pos != std::string::npos){
			std::string token = s.substr(0, pos);
			//sometimes we get a \n first then it will go bananas on an empty string
			if (token != "") {
				output.push_back(token);
			}
			s.erase(0, pos + delim.length());
			pos = s.find(delim);
		}
	}

	void Update() {
		fflush(stdout);
		//If we cant lock queue we will just try again next frame
		if (m_QueueLock.try_lock()) {
			for (uint32_t i = 0; i < m_MessageQueue.size(); ++i) {
				//Handle messages in order
				rapidjson::Document doc;
				//We can get multiple messages in 1 message because of how tcp buffers data
				//There will be a \n inserted between each message
				std::vector<std::string> messages;
				Splitstring(m_MessageQueue[i], "\n", messages);
				for (auto& message : messages) {
					doc.Parse(message.c_str());
					if (doc.HasParseError()) {
						//TODO: Expose logging api
						printf("rapidjson parse error: %d, string%s\n", doc.GetParseError(), message.c_str());
						continue;
					}
					if (doc.HasMember("Type")) {
						const char* type = doc["Type"].GetString();
						if (strcmp(type, "OpenConnection") == 0) {
							std::string reply = "{\"Type\":\"ConnectionAccepted\"}";
							m_Socket.send((void*)reply.c_str(), reply.size());
							m_Connected = true;
						} else if (strcmp(type, "FinishedSetup") == 0) {
							printf("Got FinishedSetup message\n");
							//with this we now we have no more breakpoints to set
							m_Setup = true;
						} else if (strcmp(type, "SetBreakpoints") == 0) {
							printf("Got SetBreakpoints message\n");
							if (doc.HasMember("Breakpoints")) {
								rapidjson::Value& breakpoints = doc["Breakpoints"];
								if (breakpoints.IsArray()) {
									std::string file = doc["File"].GetString();
									auto& fileBreakpoints = m_Breakpoints[file];
									fileBreakpoints.clear();
									for (auto& b : breakpoints.GetArray()) {
										AsBreakpoint bp;
										bp.lineNumber = b["Line"].GetInt();
										bp.id = b["ID"].GetInt();
										fileBreakpoints.push_back(bp);
										printf("Set breakpoint in file: %s at line: %d\n", file.c_str(), bp.lineNumber);
										//Send validation back to vscode
										rapidjson::Document doc = rapidjson::Document();
										doc.SetObject();
										doc.AddMember("Type", rapidjson::Value("ValidatedBreakpoint", doc.GetAllocator()), doc.GetAllocator());
										doc.AddMember("ID", rapidjson::Value(bp.id), doc.GetAllocator());
										rapidjson::StringBuffer buffer;
										buffer.Clear();
										rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
										doc.Accept(writer);
										m_Socket.send((void*)buffer.GetString(), buffer.GetSize());
									}
								}
							}
						} else if (strcmp(type, "StepOver") == 0) {
							m_Stepping = true;
							m_StepType = STEP_OVER;
							//If we are suspended start executing again
							if (m_Suspended) {
								m_Suspended = false;
								//We want to get back to the same function as are currently in
								m_StepFuncTarget = m_SuspendedContext->GetFunction(0);
								m_SuspendedContext->Execute();
								//we exited the angelscript function and ended back here.
								m_Stepping = false;
								m_StepFuncTarget = nullptr;
							}
						} else if (strcmp(type, "StepIn") == 0) {
							m_Stepping = true;
							m_StepType = STEP_IN;
							//If we are suspended start executing again
							if (m_Suspended) {
								m_Suspended = false;
								//We check if 1 step above the current callstack is back at this function then we have successfully stepped into the next function
								m_StepFuncTarget = m_SuspendedContext->GetFunction(0);
								m_SuspendedContext->Execute();
								m_Stepping = false;
								m_StepFuncTarget = nullptr;
							}
						} else if (strcmp(type, "StepOut") == 0) {
							m_Stepping = true;
							m_StepType = STEP_OUT;
							//If we are suspended start executing again
							if (m_Suspended) {
								m_Suspended = false;
								//We want to get back to the function 1 step above us in the callstack
								if (m_SuspendedContext->GetCallstackSize() > 1) {
									m_StepFuncTarget = m_SuspendedContext->GetFunction(1);
								}
								m_SuspendedContext->Execute();
								m_Stepping = false;
								m_StepFuncTarget = nullptr;
							}
						} else if (strcmp(type, "Continue") == 0) {
							m_Suspended = false;
							m_Stepping = false;
							m_SuspendedContext->Execute();
						}
					}
				}
			}
			m_MessageQueue.clear();
			m_QueueLock.unlock();
		}
	}

	void End() {
		m_ExitServerThread = true;
		m_Socket.close();
		m_ServerThread.join();
		m_Socket.close();
	}

	void SetContext(asIScriptContext* context) {
		//Register context callback
		context->SetLineCallback(asFUNCTION(DebugLineCallback), this, asCALL_CDECL);
	}

	bool HasBreakpoints() {
		return !m_Breakpoints.empty();
	}

	bool HasBreakpoint(uint32_t line, const char* filename) {
		auto& fileBreakpoints = m_Breakpoints.find(filename);
		if (fileBreakpoints != m_Breakpoints.end()) {
			for (auto& bp : fileBreakpoints->second) {
				if (bp.lineNumber == line)
					return true;
			}
		}
		return false;
	}

	void AddVariable(rapidjson::Value& out, rapidjson::MemoryPoolAllocator<>& allocator, int typeID, void* var, asIScriptEngine* engine) {
		switch (typeID)
		{
		case asETypeIdFlags::asTYPEID_BOOL:
			out.AddMember("Value", Value(*(bool*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_INT8:
			out.AddMember("Value", Value(*(int8_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_INT16:
			out.AddMember("Value", Value(*(int16_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_INT32:
			out.AddMember("Value", Value(*(int32_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_INT64:
			out.AddMember("Value", Value(*(int64_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_UINT8:
			out.AddMember("Value", Value(*(uint8_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_UINT16:
			out.AddMember("Value", Value(*(uint16_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_UINT32:
			out.AddMember("Value", Value(*(uint32_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_UINT64:
			out.AddMember("Value", Value(*(uint64_t*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_FLOAT:
			out.AddMember("Value", Value(*(float*)var), allocator);
			break;
		case asETypeIdFlags::asTYPEID_DOUBLE:
			out.AddMember("Value", Value(*(double*)var), allocator);
			break;
		default:
			//enum
			if ((typeID & asETypeIdFlags::asTYPEID_MASK_OBJECT) == 0) {
				if (engine) {
					asITypeInfo* info = engine->GetTypeInfoById(typeID);
					int enumCount = info->GetEnumValueCount();
					for (int i = 0; i < enumCount; ++i) {
						int enumVal;
						const char* enumName = info->GetEnumValueByIndex(i, & enumVal);
						if (enumVal == *(int*)var) {
							char buffer[64];
							sprintf(buffer, "%s:%d", enumName, enumVal);
							out.AddMember("Value", Value(buffer, allocator), allocator);
							break;
						}
					}
				}
			}//script object
			else if (typeID & asETypeIdFlags::asTYPEID_SCRIPTOBJECT) {
				if (var) {
					// Dereference handles, so we can see what it points to
					if (typeID & asTYPEID_OBJHANDLE)
						var = *(void**)var;
					if (var) {
						asIScriptObject* obj = (asIScriptObject*)var;
						asITypeInfo* info = obj->GetObjectType();
						if (info) {
							Value props = Value(kArrayType);
							int propCount = info->GetPropertyCount();
							for (int i = 0; i < propCount; ++i) {
								Value prop = Value(kObjectType);
								const char* name;
								int type;
								info->GetProperty(i, &name, &type);
								const char* propDecl = info->GetPropertyDeclaration(i, true);
								void* propAddr = obj->GetAddressOfProperty(i);
								prop.AddMember("Name", Value(name, allocator), allocator);
								prop.AddMember("Declaration", Value(propDecl, allocator), allocator);
								AddVariable(prop, allocator, type, propAddr, engine);
								props.PushBack(prop, allocator);
							}
							out.AddMember("Properties", props, allocator);
							char buffer[64];
							sprintf(buffer, "0x%x", var);
							out.AddMember("Value", Value(buffer, allocator), allocator);
						}
					} else {
						out.AddMember("Value", Value("null", allocator), allocator);
					}
					
				}
			} else {//Engine object
				char buffer[64];
				sprintf(buffer, "0x%x", var);
				out.AddMember("Value", Value(buffer, allocator), allocator);
			}
			break;
		}
	}

	void Suspend(asIScriptContext* ctx, bool breakpoint) {
		m_SuspendedContext = ctx;
		m_Suspended = true;
		m_Stepping = false;
		ctx->Suspend();

		using namespace rapidjson;
		//Send report to vscode
		//generate json message
		Document doc = Document();
		doc.SetObject();
		if (breakpoint) {
			doc.AddMember("Type", Value("HitBreakpoint", doc.GetAllocator()), doc.GetAllocator());
		} else { //Step
			doc.AddMember("Type", Value("Step", doc.GetAllocator()), doc.GetAllocator());
		}
		doc.AddMember("File", Value(ctx->GetFunction()->GetScriptSectionName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember("Line", Value(ctx->GetLineNumber()), doc.GetAllocator());

		asIScriptEngine* engine = ctx->GetEngine();
		//export callstack and variables
		Value callstack = Value(kArrayType);
		uint32_t callstackSize = ctx->GetCallstackSize();
		for (uint32_t i = 0; i < callstackSize; ++i) {
			Value trace = Value(kObjectType);
			asIScriptFunction* func = ctx->GetFunction(i);
			const char* funcNamespace = func->GetNamespace();
			const char* funcDecl = func->GetDeclaration(true, true, true);
			trace.AddMember("Declaration", Value(funcDecl, doc.GetAllocator()), doc.GetAllocator());
			const char* file = nullptr;
			trace.AddMember("Line", Value(ctx->GetLineNumber(i, nullptr, &file)), doc.GetAllocator());
			if (file) {
				trace.AddMember("File", Value(file, doc.GetAllocator()), doc.GetAllocator());
			}
			Value variables = Value(kArrayType);
			uint32_t varCount = ctx->GetVarCount(i);
			for (uint32_t k = 0; k < varCount; ++k) {
				Value var = Value(kObjectType);
				int typeID = ctx->GetVarTypeId(k, i);
				const char* varName = ctx->GetVarName(k, i);
				var.AddMember("Name", Value(varName, doc.GetAllocator()), doc.GetAllocator());
				const char* varDecl = ctx->GetVarDeclaration(k, i, true);
				var.AddMember("Declaration", Value(varDecl, doc.GetAllocator()), doc.GetAllocator());
				//For now only provide surface level values
				void* varAddr = ctx->GetAddressOfVar(k, i);
				AddVariable(var, doc.GetAllocator(), typeID, varAddr, engine);
				variables.PushBack(var, doc.GetAllocator());
				
			}
			trace.AddMember("Variables", variables, doc.GetAllocator());
			callstack.PushBack(trace, doc.GetAllocator());
		}
		doc.AddMember("CallStack", callstack, doc.GetAllocator());
		//export global variables
		asIScriptFunction* func = ctx->GetFunction(0);
		asIScriptModule* module = func->GetModule();
		Value globalVars = Value(kArrayType);
		uint32_t globalVarCount = module->GetGlobalVarCount();
		for (uint32_t i = 0; i < globalVarCount; ++i) {
			const char* name = nullptr;
			int typeID = 0;
			module->GetGlobalVar(i, &name, nullptr, &typeID);
			void* varAddr = module->GetAddressOfGlobalVar(i);
			Value var = Value(kObjectType);
			const char* varDecl = module->GetGlobalVarDeclaration(i, true);
			var.AddMember("Name", Value(name, doc.GetAllocator()), doc.GetAllocator());
			var.AddMember("Declaration", Value(varDecl, doc.GetAllocator()), doc.GetAllocator());
			AddVariable(var, doc.GetAllocator(), typeID, varAddr, engine);
			globalVars.PushBack(var, doc.GetAllocator());
		}
		doc.AddMember("GlobalVars", globalVars, doc.GetAllocator());
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		m_Socket.send((void*)buffer.GetString(), buffer.GetSize());
	}

	bool IsSuspended() {
		return m_Suspended;
	}

	bool IsStepping() {
		return m_Stepping;
	}

	bool Step(asIScriptContext* ctx) {
		if (m_StepType == STEP_OVER || m_StepType == STEP_OUT) {
			if (ctx->GetFunction(0) == m_StepFuncTarget) {
				return true;
			}
		} else if (m_StepType == STEP_IN) {
			if (ctx->GetCallstackSize() > 1) {
				if (ctx->GetFunction(1) == m_StepFuncTarget) {
					return true;
				}
			}
		}
		return false;
	}

	bool IsConnected() {
		return m_Connected;
	}

	bool IsSetup() {
		return m_Setup;
	}

private:

	void ServerThread() {
		m_Socket.listen(9202, "127.0.0.1");

		char buffer[4096];
		while (!m_ExitServerThread) {
			int dataSize = m_Socket.recieve(buffer, sizeof(buffer));
			if (dataSize > 0) {
				buffer[dataSize] = '\0';

				m_QueueLock.lock();
				m_MessageQueue.push_back(std::string(buffer));
				m_QueueLock.unlock();
			}
		}
		int i = 0;
	}

	bool m_ExitServerThread = false;
	std::thread m_ServerThread;
	Socket m_Socket;
	std::mutex m_QueueLock;
	std::vector<std::string> m_MessageQueue;
	bool m_Suspended = false;
	bool m_Connected = false;
	bool m_Setup = false;
	asIScriptContext* m_SuspendedContext = nullptr;
	std::unordered_map<std::string, std::vector<AsBreakpoint>> m_Breakpoints;
	bool m_Stepping = false;
	enum StepType {
		STEP_OVER,
		STEP_IN,
		STEP_OUT
	};
	StepType m_StepType;
	asIScriptFunction* m_StepFuncTarget = nullptr;
};

void DebugLineCallback(asIScriptContext* ctx, void* dbg) {
	ASDebugger* debugger = (ASDebugger*)dbg;
	//If we are stepping we want to hit breakpoints on the way
	if (debugger->HasBreakpoints() && (!debugger->IsSuspended() || debugger->IsStepping())) {
		asIScriptFunction* func = ctx->GetFunction();
		const char* filename = func->GetScriptSectionName();
		if (filename) {
			if (debugger->HasBreakpoint(ctx->GetLineNumber(), filename)) {
				printf("Hit breakpoint at line: %d, file:%s\n", ctx->GetLineNumber(), filename);
				//Hit a breakpoint!
				debugger->Suspend(ctx, true);
			}
		}
		
	}

	if (debugger->IsStepping()) {
		if (debugger->Step(ctx)) {
			debugger->Suspend(ctx, false);
		}
	}
}