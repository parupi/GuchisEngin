#include <GameScene.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <ParticleManager.h>
#include <imgui.h>

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
	ModelManager::GetInstance()->LoadModel("resource", "uvChecker.gltf");
	//ModelManager::GetInstance()->LoadModel("models/player/float_body.obj");
	
	object_ = new Object3d();
	object_->Initialize();
	object_->SetModel("uvChecker.gltf");

	transform_.Initialize();

	// ============ライト=================//
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);

	ParticleManager::GetInstance()->CreateParticleGroup("circle", "resource/circle.png");
	ParticleManager::GetInstance()->SetCamera(cameraManager_.GetActiveCamera().get());
	emitter_ = new ParticleEmitter();
	emitter_->Initialize("circle");
	emitter2_ = new ParticleEmitter();
	emitter2_->Initialize("circle");
}

void GameScene::Finalize()
{

}

void GameScene::Update()
{

	cameraManager_.Update();

	ParticleManager::GetInstance()->Update();
	emitter_->Update({5.0f, 0.0f, 0.0f}, 3);
	emitter2_->Update({-5.0f, 0.0f, 0.0f}, 3);
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

	Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
	Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };
	Quaternion identity = Quaternion::Identity();
	Quaternion conj = q1.Conjugate();
	Quaternion inv = q1.Inverse();
	Quaternion normal = q1.Unit();
	Quaternion mul1 = q1 * q2;
	Quaternion mul2 = q2 * q1;
	float norm = q1.Norm();

	identity.DisplayInImGui("Identity");
	conj.DisplayInImGui("Conjugate");
	inv.DisplayInImGui("Inverse");
	normal.DisplayInImGui("Normalize");
	mul1.DisplayInImGui("Multiply(q1, q2)");
	mul2.DisplayInImGui("Multiply(q2, q1)");
	ImGui::Begin("Quaternion");
	ImGui::Text("%f", norm);
	ImGui::End();
}

void GameScene::Draw()
{
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();

	object_->Draw(transform_);
	ParticleManager::GetInstance()->Draw();
}