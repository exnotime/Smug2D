#include "EntityManager.h"
#include <ComponentManager.h>

EntityManager::EntityManager() {}

EntityManager::~EntityManager() {}

EntityManager& EntityManager::GetInstance() {
	static EntityManager* instance = nullptr;
	if (!instance) {
		instance = new EntityManager();
	}
	return *instance;
}

Entity& EntityManager::CreateEntity() {
	Entity e;
	e.ComponentBitfield = 0;
	e.UID = m_Counter++;
	m_Entities.push_back(e);
	return m_Entities.back();
}

Entity& EntityManager::GetEntity(uint32_t UID) {
	return m_Entities[UID];
}

void EntityManager::RemoveEntity(Entity& entity) {
	//potentially slow
	for (int i = 0; i < m_Entities.size(); i++) {
		if (m_Entities[i].UID == entity.UID) {
			m_DirtyComponents |= entity.ComponentBitfield;
			ComponentManager::GetInstance().RemoveComponents(m_Entities[i]);
			m_Entities.erase( m_Entities.begin() + i);
			return;
		}
	}
}

void EntityManager::RemoveAllEntities() {
	for (int i = 0; i < m_Entities.size(); i++) {
		ComponentManager::GetInstance().RemoveComponents(m_Entities[i]);
	}
	m_Entities.clear();
}

std::vector<Entity>& EntityManager::GetEntityList() {
	return m_Entities;
}

bool EntityManager::IsCacheDirty(const EntityCache& cache) {
	return (cache.ComponentBitMask & m_DirtyComponents) == cache.ComponentBitMask;
}

void EntityManager::UpdateCache(EntityCache& cache) {
	cache.Entities.clear();
	for (int i = 0; i < m_Entities.size(); i++) {
		Entity& e = m_Entities[i];
		if ((e.ComponentBitfield & cache.ComponentBitMask) == cache.ComponentBitMask) {
			cache.Entities.push_back(e.UID);
		}
	}

}