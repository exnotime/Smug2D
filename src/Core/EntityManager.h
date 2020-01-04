#pragma once
#include "Entity.h"
#include <vector>
struct EntityCache {
	uint64_t ComponentBitMask;
	std::vector<uint32_t> Entities;
	bool Dirty = false;
};

class EntityManager {
  public:
	static EntityManager& GetInstance();
	Entity& CreateEntity();
	Entity& GetEntity(uint32_t UID);
	void RemoveEntity(Entity& entity);
	void RemoveAllEntities();
	std::vector<Entity>& GetEntityList();
	bool IsCacheDirty(const EntityCache& cache);
	void UpdateCache(EntityCache& cache);
  private:
	EntityManager();
	~EntityManager();
	std::vector<Entity> m_Entities;
	uint32_t m_Counter = 0;
	uint64_t m_DirtyComponents = 0;
};