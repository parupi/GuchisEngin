#pragma once
#include <memory>
#include "charcter/enemy/Enemy.h"
#include "effect/HitStopManager.h"
class EnemyManager
{
public:
	void Initialize(Player* player, HitStopManager* hitStop);

	void Update();

	void Draw();
	void DrawSprite();

	void SetHitStop();
private:
	std::array<std::unique_ptr<Enemy>, 10> enemy_;
	Camera* camera_ = nullptr;
	Player* player_;
	HitStopManager* hitStop_;

	uint32_t deadNum_ = 0;

public:
	Enemy* GetEnemy(int i) { return enemy_[i].get(); }

	uint32_t GetDeadNum() { return deadNum_; }
	void SetDeadNum(uint32_t deadNum) { deadNum_ = deadNum; }

	void SetCamera(Camera* camera);
};

