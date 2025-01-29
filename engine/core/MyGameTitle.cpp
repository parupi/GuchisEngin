#include "MyGameTitle.h"
#include <SceneFactory.h>
#include <ParticleManager.h>
#include "LightManager.h"

void MyGameTitle::Initialize()
{
	GuchisFramework::Initialize();
	// ImGui初期化
	ImGuiManager::GetInstance()->Initialize(winManager.get(), dxManager.get());
	// 2Dテクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxManager.get(), srvManager.get());
	// 3Dテクスチャマネージャーの初期化
	ModelManager::GetInstance()->Initialize(dxManager.get(), srvManager.get());
	// パーティクルマネージャーの初期化
	ParticleManager::GetInstance()->Initialize(dxManager.get(), srvManager.get(), psoManager.get());
	// スプライト共通部の初期化
	SpriteManager::GetInstance()->Initialize(dxManager.get(), psoManager.get());
	// オブジェクト共通部
	Object3dManager::GetInstance()->Initialize(dxManager.get(), psoManager.get());
	// ライトの生成
	LightManager::GetInstance()->Initialize(dxManager.get());

	offScreen_ = std::make_unique<OffScreen>();
	offScreen_->Initialize(dxManager.get(), psoManager.get());

	// 最初のシーンを生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	// シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	// シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("GAMEPLAY");

	// インスタンス生成
	GlobalVariables::GetInstance();

	LoadFile();

	CreateParticleGroup();
}

void MyGameTitle::Finalize()
{
	ParticleManager::GetInstance()->Finalize();
	SpriteManager::GetInstance()->Finalize();
	Object3dManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();
	ImGuiManager::GetInstance()->Finalize();
	GuchisFramework::Finalize();
}

void MyGameTitle::Update()
{
	ImGuiManager::GetInstance()->Begin();
	GuchisFramework::Update();

	GlobalVariables::GetInstance()->Update();

	ImGuiManager::GetInstance()->End();
}

void MyGameTitle::Draw()
{
	dxManager->BeginDrawForRenderTarget();
	srvManager->BeginDraw();
	SceneManager::GetInstance()->Draw();


	dxManager->BeginDraw();

	offScreen_->Draw(OffScreenEffectType::kNone);

	ImGuiManager::GetInstance()->Draw();

	dxManager->EndDraw();
}

void MyGameTitle::LoadFile()
{
	ModelManager::GetInstance()->LoadModel("resource", "plane.obj");
	ModelManager::GetInstance()->LoadModel("resource", "float_body.obj");
	ModelManager::GetInstance()->LoadModel("resource", "float_head.obj");
	ModelManager::GetInstance()->LoadModel("resource", "ICO.obj");
	ModelManager::GetInstance()->LoadModel("resource", "sord/sord.obj");
	ModelManager::GetInstance()->LoadModel("resource", "models/player/player.obj");

	TextureManager::GetInstance()->LoadTexture("resource/UI/TitleUI.png");
	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resource/damageSprite.png");
}

void MyGameTitle::CreateParticleGroup()
{
	ParticleManager::GetInstance()->CreateParticleGroup("Dust", "resource/circle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("Attack", "resource/fire.png");

}

