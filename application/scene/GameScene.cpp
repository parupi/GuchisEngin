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
	ModelManager::GetInstance()->LoadModel("Resource", "walk.gltf");
	ModelManager::GetInstance()->LoadModel("Resource", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("Resource", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("Resource", "plane.obj");
	ModelManager::GetInstance()->LoadModel("Resource", "Models/AnimatedCube/AnimatedCube.gltf");
	ModelManager::GetInstance()->LoadModel("Resource", "Models/Terrain/Terrain.obj");
	ModelManager::GetInstance()->LoadModel("Resource", "multiMaterial.obj");
	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

	object_ = std::make_unique<Object3d>();
	object_->Initialize("Models/AnimatedCube/AnimatedCube.gltf");

	animationObject_ = std::make_unique<Object3d>();
	animationObject_->Initialize("simpleSkin.gltf");

	//transform_.Initialize();
	//animationTransform_.Initialize();

	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Resource/uvChecker.png");
	sprite->SetSize({ 32.0f,32.0f });

	// ============ライト=================//
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);


	ParticleManager::GetInstance()->CreateParticleGroup("test", "Resource/uvChecker.png");

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

	
	DebugUpdate();

	Vector2 uvObjectPos = object_->GetUVPosition();
	Vector2 uvObjectSize = object_->GetUVSize();
	float uvObjectRotate = object_->GetUVRotation();

	ImGui::Begin("Transform");
	ImGui::DragFloat2("UVTranslate", &uvObjectPos.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvObjectSize.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvObjectRotate);
	ImGui::End();

	Quaternion rotate = MakeRotateAxisAngleQuaternion(axis, angle);

	PrintOnImGui(object_->GetWorldTransform()->GetMatWorld());

	object_->SetUVPosition(uvObjectPos);
	object_->SetUVSize(uvObjectSize);
	object_->SetUVRotation(uvObjectRotate);

	object_->Update();

	animationObject_->Update();

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

	Vector2 spritePos = sprite->GetPosition();
	Vector2 spriteSize = sprite->GetSize();
	float spriteRotate = sprite->GetRotation();

	Vector2 uvSpritePos = sprite->GetUVPosition();
	Vector2 uvSpriteSize = sprite->GetUVSize();
	float uvSpriteRotate = sprite->GetUVRotation();

	ImGui::Begin("Sprite");
	ImGui::DragFloat2("position", &spritePos.x);
	ImGui::DragFloat("rotate", &spriteRotate);
	ImGui::DragFloat2("size", &spriteSize.x);
	ImGui::DragFloat2("UVTranslate", &uvSpritePos.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvSpriteSize.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvSpriteRotate);
	ImGui::End();

	sprite->SetPosition(spritePos);
	sprite->SetSize(spriteSize);
	sprite->SetRotation(spriteRotate);

	sprite->SetUVPosition(uvSpritePos);
	sprite->SetUVSize(uvSpriteSize);
	sprite->SetUVRotation(uvSpriteRotate);
}

void GameScene::Draw()
{
	ParticleManager::GetInstance()->DrawSet();
	ParticleManager::GetInstance()->Draw();

	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSetForAnimation();
	lightManager_->BindLightsToShader();
	animationObject_->Draw();

	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();
	object_->Draw();

	SpriteManager::GetInstance()->DrawSet();
	sprite->Draw();
	
}

void GameScene::DrawRTV()
{
}

#ifdef _DEBUG
void GameScene::DebugUpdate()
{
	ImGui::Begin("Object");
	object_->DebugGui();
	ImGui::End();
}
#endif // _DEBUG
