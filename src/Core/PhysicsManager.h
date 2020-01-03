#pragma once

class PhysicsManager {
public:
	static PhysicsManager& GetInstance();
	void Init();
private:
	PhysicsManager();
	~PhysicsManager();
};