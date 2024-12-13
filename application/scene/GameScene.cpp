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
	ModelManager::GetInstance()->LoadModel("resource", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "test.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "AnimatedCube.gltf");
	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

	object_ = new Object3d();
	//object_->Initialize("plane.obj");
	object_->Initialize("walk.gltf");
	//object_->Initialize("uvChecker.gltf");
	//object_->Initialize("AnimatedCube.gltf");

	transform_.Initialize();

	sprite = new Sprite();
	sprite->Initialize("resource/uvChecker.png");

	// ============ライト=================//
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);

	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize();
	particleManager_->CreateParticleGroup("test", "resource/uvChecker.png");
}

void GameScene::Finalize()
{
	delete object_;
	delete sprite;
}

void GameScene::Update()
{
	particleManager_->Emit("test", transform_.translation_, 1);
	particleManager_->Update();

	object_->AnimationUpdate();
	cameraManager_.Update();

	sprite->Update();

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

	ImGui::Begin("Transform");
	ImGui::DragFloat3("translate", &transform_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &transform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.01f);
	ImGui::End();

	transform_.TransferMatrix();

	ImGui::Begin("SetModel");
	if (ImGui::Button("Set Work"))
	{
		object_->SetModel("walk.gltf");
	}
	if (ImGui::Button("Set sneakWalk"))
	{
		object_->SetModel("sneakWalk.gltf");
	}
	ImGui::End();
}

void GameScene::Draw()
{
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSetForAnimation();
	lightManager_->BindLightsToShader();
	object_->Draw(transform_);

	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();
	
	SpriteManager::GetInstance()->DrawSet();
	sprite->Draw();

	ParticleResources::GetInstance()->DrawSet();
	particleManager_->Draw();
}