#pragma once
#include "ComponentBuffer.h"
#include "Entity.h"
#include <vector>

class ComponentManager {
  public:
	static ComponentManager& GetInstance();
	void AddComponentType(uint32_t maxCount, uint32_t size, uint64_t componentID, const char* name = "");
	void CreateComponent(const void* comp, Entity& ent, uint64_t type);
	void RemoveComponent(Entity& ent, uint64_t type);
	void RemoveComponents(Entity& ent);
	int GetBuffer(void** outBuffer, uint64_t type);
	void* GetComponent(const Entity& ent, uint64_t type);
	int GetComponentTypeCount() const { return m_ComponentTypeCount; }
  private:
	  ComponentManager();
	  ~ComponentManager();
	void CreateComponentBuffer(uint32_t count, uint32_t componentSize, uint64_t id, std::string name);
	std::vector<ComponentBuffer>	m_Buffers;
	int								m_ComponentTypeCount;
};