#include "Components.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include <angelscript.h>
using namespace AngelScript;
namespace Components {


	void CreateTransformComponentFull(uint32_t uid, Vec2 pos, Vec2 scale, Vec2 localOrigin, float rotation) {
		TransformComponent tc;
		tc.position = pos;
		tc.scale = scale;
		tc.localOrigin = localOrigin;
		tc.rotation = rotation;
		Entity& e = EntityManager::GetInstance().GetEntity(uid);
		ComponentManager::GetInstance().CreateComponent(&tc, e, ComponentType::TRANSFORM);
	}

	void CreateTransformComponent(uint32_t uid) {
		TransformComponent tc;
		Entity& e = EntityManager::GetInstance().GetEntity(uid);
		ComponentManager::GetInstance().CreateComponent(&tc, e, ComponentType::TRANSFORM);
	}

	TransformComponent* GetTransformComponent(uint32_t uid) {
		Entity& e = EntityManager::GetInstance().GetEntity(uid);
		return (TransformComponent*)ComponentManager::GetInstance().GetComponent(e, ComponentType::TRANSFORM);
	}

	void CreateSpriteComponentFull(uint32_t uid, sf::Texture* texture, int layer, Rect textureRect, sf::Color color) {
		SpriteComponent sc;
		sc.texture = texture;
		sc.layer = layer;
		sc.textureRect = { 0.0f, 0.0f, (float)texture->getSize().x, (float)texture->getSize().y };
		sc.tint = color;
		Entity& e = EntityManager::GetInstance().GetEntity(uid);
		ComponentManager::GetInstance().CreateComponent(&sc, e, ComponentType::SPRITE);
	}

	void CreateSpriteComponent(uint32_t uid) {
		SpriteComponent sc;
		Entity& e = EntityManager::GetInstance().GetEntity(uid);
		ComponentManager::GetInstance().CreateComponent(&sc, e, ComponentType::SPRITE);
	}

	SpriteComponent* GetSpriteComponent(uint32_t uid) {
		Entity& e = EntityManager::GetInstance().GetEntity(uid);
		return (SpriteComponent*)ComponentManager::GetInstance().GetComponent(e, ComponentType::SPRITE);
	}

	void LoadComponentInterface(asIScriptEngine* engine) {
		ComponentManager& cm = ComponentManager::GetInstance();
		cm.AddComponentType(100, sizeof(TransformComponent), ComponentType::TRANSFORM);
		cm.AddComponentType(100, sizeof(SpriteComponent), ComponentType::SPRITE);
		cm.AddComponentType(100, sizeof(PhysicsComponent), ComponentType::PHYSICS);

		engine->RegisterEnum("ComponentType");
		engine->RegisterEnumValue("ComponentType", "Transform", ComponentType::TRANSFORM);
		engine->RegisterEnumValue("ComponentType", "Sprite", ComponentType::SPRITE);

		engine->RegisterObjectType("TransformComponent", sizeof(TransformComponent), asOBJ_REF | asOBJ_NOCOUNT);
		engine->RegisterObjectProperty("TransformComponent", "Vec2 position", asOFFSET(TransformComponent, position));
		engine->RegisterObjectProperty("TransformComponent", "Vec2 scale", asOFFSET(TransformComponent, scale));
		engine->RegisterObjectProperty("TransformComponent", "Vec2 localOrigin", asOFFSET(TransformComponent, localOrigin));
		engine->RegisterObjectProperty("TransformComponent", "float rotation", asOFFSET(TransformComponent, rotation));

		engine->RegisterObjectType("SpriteComponent", sizeof(SpriteComponent), asOBJ_REF | asOBJ_NOCOUNT);
		engine->RegisterObjectProperty("SpriteComponent", "Texture@ texture", asOFFSET(SpriteComponent, texture));
		engine->RegisterObjectProperty("SpriteComponent", "int layer", asOFFSET(SpriteComponent, layer));
		engine->RegisterObjectProperty("SpriteComponent", "Rect textureRect", asOFFSET(SpriteComponent, textureRect));
		engine->RegisterObjectProperty("SpriteComponent", "Color color", asOFFSET(SpriteComponent, tint));

		engine->RegisterGlobalFunction("void CreateTransformComponent(EntityHandle e, Vec2 pos, Vec2 scale, Vec2 localOrigin, float rotation)", asFUNCTION(CreateTransformComponentFull), asCALL_CDECL);
		engine->RegisterGlobalFunction("void CreateTransformComponent(EntityHandle e)", asFUNCTION(CreateTransformComponent), asCALL_CDECL);
		engine->RegisterGlobalFunction("TransformComponent@ GetTransformComponent(EntityHandle e)", asFUNCTION(GetTransformComponent), asCALL_CDECL);
		engine->RegisterGlobalFunction("void CreateSpriteComponent(EntityHandle e, Texture@ texture, int layer, Rect textureRect, Color color)", asFUNCTION(CreateSpriteComponentFull), asCALL_CDECL);
		engine->RegisterGlobalFunction("void CreateSpriteComponent(EntityHandle e)", asFUNCTION(CreateSpriteComponent), asCALL_CDECL);
		engine->RegisterGlobalFunction("SpriteComponent@ GetSpriteComponent(EntityHandle e)", asFUNCTION(GetSpriteComponent), asCALL_CDECL);
	}
}