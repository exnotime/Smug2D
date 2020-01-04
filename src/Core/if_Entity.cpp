#include "if_Entity.h"
#include "Entity.h"
#include "EntityManager.h"
#include <angelscript.h>
#include <new>
using namespace AngelScript;

Entity* CreateEntity() {
	return &EntityManager::GetInstance().CreateEntity();
}

void DestroyEntity(Entity* e) {
	EntityManager::GetInstance().RemoveEntity(*e);
}

void if_entity::LoadEntityInterface(asIScriptEngine* engine) {
	engine->RegisterObjectType("Entity", sizeof(Entity), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterGlobalFunction("Entity@ CreateEntity()", asFUNCTION(CreateEntity), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DestroyEntity(Entity@ e)", asFUNCTION(DestroyEntity), asCALL_CDECL);
}