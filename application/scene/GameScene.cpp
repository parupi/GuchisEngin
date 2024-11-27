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
	//ModelManager::GetInstance()->LoadModel("models/player/float_body.obj");
	
	object_ = new Object3d();
	object_->Initialize("walk.gltf");
	//object_->SetModel("uvChecker.gltf");

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

	Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
	Quaternion rotation1 = { -rotation0.x, -rotation0.y, -rotation0.z, -rotation0.w };

	Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

	PrintOnImGui(interpolate0);
	PrintOnImGui(interpolate1);
	PrintOnImGui(interpolate2);
	PrintOnImGui(interpolate3);
	PrintOnImGui(interpolate4);
}

void GameScene::Draw()
{
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();

	object_->Draw(transform_);
	//ParticleManager::GetInstance()->Draw();
}