#include "if_Entity.h"
#include "Entity.h"
#include "EntityManager.h"
#include <angelscript.h>
#include <new>
using namespace AngelScript;

uint32_t CreateEntity() {
	return EntityManager::GetInstance().CreateEntity().UID;
}

void DestroyEntity(uint32_t uid) {
	EntityManager::GetInstance().RemoveEntity(EntityManager::GetInstance().GetEntity(uid));
}

uint64_t GetEntityComponents(uint32_t uid) {
	return EntityManager::GetInstance().GetEntity(uid).ComponentBitfield;
}

void if_entity::LoadEntityInterface(asIScriptEngine* engine) {
	engine->RegisterTypedef("EntityHandle", "uint");
	engine->RegisterGlobalFunction("EntityHandle CreateEntity()", asFUNCTION(CreateEntity), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DestroyEntity(EntityHandle e)", asFUNCTION(DestroyEntity), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 GetEntityComponents(EntityHandle e)", asFUNCTION(GetEntityComponents), asCALL_CDECL);
}