#include <GameScene.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <ParticleManager.h>
#include <imgui.h>
#include "SceneManager.h"

void GameScene::Initialize()
{
	hitStop_ = std::make_unique<HitStopManager>();

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize(hitStop_.get());

	gameCamera_->SetPlayer(player_.get());
	player_->SetCamera(gameCamera_->GetGameCamera());


	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(player_.get(), hitStop_.get());
	enemyManager_->SetCamera(gameCamera_->GetGameCamera());

	ground_ = std::make_unique<Ground>();
	ground_->Initialize();

	sphere_ = std::make_unique<Skydome>();
	sphere_->Initialize();

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);

	TextureManager::GetInstance()->LoadTexture("resource/UI/tutorialUI.png");
	titleUI_ = std::make_unique<Sprite>();
	titleUI_->Initialize("resource/UI/tutorialUI.png");
}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
	hitStop_->Update();

	if (hitStop_->IsHitStopActive()) {
		return;
	}

	ParticleManager::GetInstance()->Update();
	gameCamera_->Update();

	player_->Update();

	enemyManager_->Update();
	enemyManager_->SetHitStop();

	sphere_->Update();
	ground_->Update();

	titleUI_->Update();

	CheckAllCollisions();


	if (enemyManager_->GetDeadNum() == 10) {
		// シーンの切り替え依頼
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
}

void GameScene::Draw()
{
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSet();
	ground_->Draw();
	sphere_->Draw();
	enemyManager_->Draw();
	player_->Draw();

	SpriteManager::GetInstance()->DrawSet(BlendMode::kAdd);
	titleUI_->Draw();
	enemyManager_->DrawSprite();

	ParticleManager::GetInstance()->DrawSet(BlendMode::kAdd);
	ParticleManager::GetInstance()->Draw();
}

void GameScene::CheckAllCollisions()
{
	// 衝突マネージャーのリセット
	collisionManager_->Reset();

	// コライダーリストに登録
	collisionManager_->AddCollider(player_->GetWeapon());

	// 敵全てについて
	 // 敵全てのコライダーをリストに登録
	//auto enemyColliders = enemyGroup_->GetColliders();
	for (int i = 0; i < 10; i++) {
		collisionManager_->AddCollider(enemyManager_->GetEnemy(i));
	}


	// 衝突判定と応答
	collisionManager_->CheckAllCollisions();
}
