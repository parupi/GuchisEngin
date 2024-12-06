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
	normalCamera_->SetTranslate(Vector3{ 0.0f, 10.0f, -30.0f });
	bossCamera_->SetTranslate(Vector3{ 0.0f, 0.0f, -100.0f });

	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("resource", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("resource", "sneakWalk.gltf");
	//ModelManager::GetInstance()->LoadModel("resource", "plane.obj");
	ModelManager::GetInstance()->LoadModel("resource", "AnimatedCube.gltf");

	object_ = new Object3d();
	//object_->Initialize("plane.obj");
	object_->Initialize("simpleSkin.gltf");
	//object_->Initialize("uvChecker.gltf");
	//object_->Initialize("AnimatedCube.gltf");

	transform_.Initialize();

	// ============ライト=================//
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);

	//ParticleManager::GetInstance()->CreateParticleGroup("circle", "resource/circle.png");
	//ParticleManager::GetInstance()->CreateParticleGroup("test", "resource/circle.png");
	//ParticleManager::GetInstance()->SetCamera(cameraManager_.GetActiveCamera().get());
	//emitter_ = new ParticleEmitter();
	//emitter_->Initialize("circle");
	//emitter2_ = new ParticleEmitter();
	//emitter2_->Initialize("test");

	//ParticleManager::GetInstance()->SetParticleVelocityLange("test", -10.0f, 10.0f);

	for (uint32_t i = 0; i < object_->GetModel()->GetConnectionCount(); i++) {
		DebugSphere* sphere = new DebugSphere();
		sphere->Initialize();
		spheres_.push_back(sphere);
	}
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

	for (size_t i = 0; i < spheres_.size(); i++) {
		spheres_[i]->Update(object_->GetModel()->GetConnectionPositions()[i]);
	}

}

void GameScene::Draw()
{
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSetForAnimation();
	lightManager_->BindLightsToShader();

	object_->Draw(transform_);
	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();
	
	for (size_t i = 0; i < spheres_.size(); i++) {
		spheres_[i]->Draw();
	}


	//ParticleManager::GetInstance()->Draw();
}