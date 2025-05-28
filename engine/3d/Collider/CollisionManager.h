#pragma once
#include <mutex>
#include <vector>
#include "BaseCollider.h"
class CollisionManager
{
private:
	static CollisionManager* instance;
	static std::once_flag initInstanceFlag;

	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(CollisionManager&) = default;
	CollisionManager& operator=(CollisionManager&) = default;

public:
	static CollisionManager* GetInstance();

	void Initialize();
	// 終了処理
	void Finalize();
	void Update();

	void Draw();

	void AddCollider(std::unique_ptr<BaseCollider> collider);

	BaseCollider* FindCollider(std::string colliderName);
private:

	
	//void RemoveCollider(BaseCollider* collider);
	//void CheckAllCollisions();
	//bool CheckCollision(const BaseCollider* a, const BaseCollider* b);

private:
	std::vector<std::unique_ptr<BaseCollider>> colliders_;

};

