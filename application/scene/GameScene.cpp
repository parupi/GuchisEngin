#include <GameScene.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <ParticleManager.h>
#include <imgui.h>
#include <Quaternion.h>
#include <Vector3.h>
#include <Matrix4x4.h>

void GameScene::Initialize()
{
	// カメラの生成
	normalCamera_ = std::make_shared<Camera>();
	bossCamera_ = std::make_shared<Camera>();
	cameraManager_.AddCamera(normalCamera_);
	cameraManager_.AddCamera(bossCamera_);
	cameraManager_.SetActiveCamera(0);
	normalCamera_->SetTranslate(Vector3{ 0.0f, 35.0f, -44.0f });
	normalCamera_->SetRotate(Vector3{ 0.68f, 0.0f, 0.0f });
	bossCamera_->SetTranslate(Vector3{ 0.0f, 0.0f, -100.0f });

	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("resource", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "plane.obj");
	
	object_ = new Object3d();
	object_->Initialize("plane.obj");
	//object_->SetModel("uvChecker.gltf");

	transform_.Initialize();

	// ============ライト=================//
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);

	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize();
	particleManager_->CreateParticleGroup("snow", "resource/snow.png");
	snowEmitter_ = std::make_unique<ParticleEmitter>();
	snowEmitter_->Initialize(particleManager_.get(), "snow");
}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
	object_->AnimationUpdate();
	cameraManager_.Update();

	//ParticleManager::GetInstance()->Update();
	//emitter_->Update({5.0f, 0.0f, 0.0f}, 10000);
	//emitter2_->Update({-5.0f, 0.0f, 0.0f}, 10000);

	Vector3 normalCameraPos = normalCamera_->GetTranslate();
	Vector3 bossCameraPos = bossCamera_->GetTranslate();

	ImGui::Begin("Camera Manager");
	ImGui::DragFloat3("normalPos", &normalCameraPos.x, 0.01f);
	ImGui::DragFloat3("bossPos", &bossCameraPos.x, 0.01f);

	if (ImGui::Button("Set Camera 1"))
	{
		cameraManager_.SetActiveCamera(0);
	}
	if (ImGui::Button("Set Camera 2"))
	{
		cameraManager_.SetActiveCamera(1);
	}
	ImGui::End();

	normalCamera_->SetTranslate(normalCameraPos);
	bossCamera_->SetTranslate(bossCameraPos);

	transform_.TransferMatrix();

	particleManager_->Update();
	snowEmitter_->Update({ 0.0f, 2.0f, 0.0f }, 10);
}

void GameScene::Draw()
{
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSetForAnimation();
	lightManager_->BindLightsToShader();


	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();

	//object_->Draw(transform_);

	ParticleResources::GetInstance()->DrawSet();
	particleManager_->Draw();
}