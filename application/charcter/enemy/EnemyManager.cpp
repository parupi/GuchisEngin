#include "EnemyManager.h"
#include "charcter/player/Player.h"

void EnemyManager::Initialize(Player* player, HitStopManager* hitStop)
{
	player_ = player;
	hitStop_ = hitStop;

	for (int i = 0; i < 10; i++) {
		enemy_[i] = std::make_unique<Enemy>();
		enemy_[i]->Initialize({ -30.0f , 0.0f, (i - 5) * 20.0f });
		enemy_[i]->SetPlayer(player_);
	}
}

void EnemyManager::Update()
{
	for (int i = 0; i < 10; i++) {
		enemy_[i]->Update();
		if (enemy_[i]->IsDeadTriger()) {
			deadNum_++;
		}
	}
}

void EnemyManager::Draw()
{
	for (int i = 0; i < 10; i++) {
		enemy_[i]->Draw();
	}
}

void EnemyManager::DrawSprite()
{
	for (int i = 0; i < 10; i++) {
		enemy_[i]->DrawSprite();
	}
}

void EnemyManager::SetHitStop()
{
	bool isHit = player_->GetIsHit();
	for (int i = 0; i < 10; i++) {
		if (isHit && !enemy_[i]->GetIsHit()) {
			hitStop_->Trigger();
			return;
		}
	}
}

void EnemyManager::SetCamera(Camera* camera)
{
	camera_ = camera;
	for (int i = 0; i < 10; i++) {
		enemy_[i]->SetCamera(camera_);
	}
}
