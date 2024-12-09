#include "TitleScene.h"
#include <Object3dManager.h>
#include <Input.h>
#include <GameScene.h>

void TitleScene::Initialize()
{
	TextureManager::GetInstance()->LoadTexture("resource/fade1x1.png");

	// カメラの生成
	camera_ = std::make_shared<Camera>();
	cameraManager_.AddCamera(camera_);
	cameraManager_.AddCamera(camera_);
	cameraManager_.SetActiveCamera(0);
	camera_->SetTranslate(Vector3{ 0.0f, 35.0f, -44.0f });
	camera_->SetRotate(Vector3{ 0.68f, 0.0f, 0.0f });

	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Status::FadeIn, 2.0f);

	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize();
	particleManager_->CreateParticleGroup("snow", "resource/snow.png");
	snowEmitter_ = std::make_unique<ParticleEmitter>();
	snowEmitter_->Initialize(particleManager_.get(), "snow");
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	cameraManager_.Update();

	fade_->Update();

	ChangePhase();

	particleManager_->Update();
	snowEmitter_->Update({ 0.0f, 2.0f, 0.0f }, 10);
}

void TitleScene::Draw()
{
	Object3dManager::GetInstance()->DrawSet();
	fade_->Draw();

	ParticleResources::GetInstance()->DrawSet();
	particleManager_->Draw();

	SpriteManager::GetInstance()->DrawSet();
	fade_->DrawSprite();
}

void TitleScene::ChangePhase()
{
	switch (phase_) {
	case TitlePhase::kTitle:
		// キーボードの処理
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = TitlePhase::kFadeOut;
			fade_->Start(Status::FadeOut, 2.0f);
		}
		break;
	case TitlePhase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = TitlePhase::kTitle;
		}
		break;
	case TitlePhase::kFadeOut:
		if (fade_->IsFinished()) {
			// シーンの切り替え依頼
			//SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
			phase_ = TitlePhase::kFadeIn;
			fade_->Start(Status::FadeIn, 2.0f);

		}
		break;
	}

}
