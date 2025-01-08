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
	cameraManager_.AddCamera(normalCamera_);
	cameraManager_.SetActiveCamera(0);
	normalCamera_->SetTranslate(Vector3{ 0.0f, 35.0f, -44.0f });
	normalCamera_->SetRotate(Vector3{ 0.68f, 0.0f, 0.0f });
	//normalCamera_->SetTranslate(Vector3{ 0.0f, 0.0f, -10.0f });
	//normalCamera_->SetRotate(Vector3{ 0.0f, 0.0f, 0.0f });

	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("resource", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "plane.obj");
	ModelManager::GetInstance()->LoadModel("resource", "AnimatedCube.gltf");
	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

	object_ = std::make_unique<Object3d>();
	object_->Initialize("plane.obj");

	animationObject_ = std::make_unique<Object3d>();
	animationObject_->Initialize("walk.gltf");

	transform_.Initialize();
	animationTransform_.Initialize();

	sprite = std::make_unique<Sprite>();
	sprite->Initialize("resource/uvChecker.png");
	sprite->SetSize({ 32.0f,32.0f });

	// ============ライト=================//
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);


	ParticleManager::GetInstance()->CreateParticleGroup("test", "resource/uvChecker.png");

	particleEmitter_ = std::make_unique<ParticleEmitter>();
	particleEmitter_->Initialize("test");

}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
	particleEmitter_->Update({0.0f, 0.0f, 0.0f}, 8);
	ParticleManager::GetInstance()->Update();

	object_->AnimationUpdate();
	animationObject_->AnimationUpdate();
	cameraManager_.Update();
	sprite->Update();

	Vector3 normalCameraPos = normalCamera_->GetTranslate();
	Vector3 cameraRotate = normalCamera_->GetRotate();

	ImGui::Begin("Camera Manager");
	ImGui::DragFloat3("Translate", &normalCameraPos.x, 0.01f);
	ImGui::DragFloat3("Rotate", &cameraRotate.x, 0.01f);
	ImGui::End();

	normalCamera_->SetTranslate(normalCameraPos);
	normalCamera_->SetRotate(cameraRotate);

	ImGui::Begin("Transform");
	ImGui::DragFloat3("translate", &transform_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &transform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.01f);
	ImGui::End();

	transform_.TransferMatrix();

	ImGui::Begin("AnimationTransform");
	ImGui::DragFloat3("translate", &animationTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &animationTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("scale", &animationTransform_.scale_.x, 0.01f);
	ImGui::End();

	animationTransform_.TransferMatrix();

	ImGui::Begin("SetModel");
	if (ImGui::Button("Set Work"))
	{
		animationObject_->SetModel("walk.gltf");
	}
	if (ImGui::Button("Set sneakWalk"))
	{
		animationObject_->SetModel("sneakWalk.gltf");
	}
	ImGui::End();
}

void GameScene::Draw()
{
	ParticleManager::GetInstance()->DrawSet();
	ParticleManager::GetInstance()->Draw();

	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSetForAnimation();
	lightManager_->BindLightsToShader();
	animationObject_->Draw(animationTransform_);

	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();
	object_->Draw(transform_);

	SpriteManager::GetInstance()->DrawSet();
	sprite->Draw();
	
}