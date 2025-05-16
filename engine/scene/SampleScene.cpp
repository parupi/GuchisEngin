#include "SampleScene.h"
#include <TextureManager.h>
#include <Model/ModelManager.h>
#include <ParticleManager.h>
#include <imgui.h>
#include <Quaternion.h>
#include <Vector3.h>
#include <Matrix4x4.h>
#include <offscreen/OffScreenManager.h>
#include <offscreen/VignetteEffect.h>
#include <offscreen/SmoothEffect.h>
#include <Primitive/PrimitiveDrawer.h>

void SampleScene::Initialize()
{
	// カメラの生成
	normalCamera_ = std::make_shared<Camera>();
	cameraManager_.AddCamera(normalCamera_);
	cameraManager_.SetActiveCamera(0);
	normalCamera_->SetTranslate(Vector3{ 0.0f, 35.0f, -44.0f });
	normalCamera_->SetRotate(Vector3{ 0.68f, 0.0f, 0.0f });


	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadSkinnedModel("walk");
	ModelManager::GetInstance()->LoadSkinnedModel("simpleSkin");
	ModelManager::GetInstance()->LoadSkinnedModel("sneakWalk");
	ModelManager::GetInstance()->LoadSkinnedModel("ParentKoala");

	ModelManager::GetInstance()->LoadModel("plane");
	ModelManager::GetInstance()->LoadModel("Terrain");
	ModelManager::GetInstance()->LoadModel("axis");
	ModelManager::GetInstance()->LoadModel("ICO");
	ModelManager::GetInstance()->LoadModel("multiMesh");
	ModelManager::GetInstance()->LoadModel("multiMaterial");
	TextureManager::GetInstance()->LoadTexture("uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("gradationLine.png");

	object_ = std::make_unique<Object3d>();
	object_->Initialize("multiMaterial");

	animationObject_ = std::make_unique<Object3d>();
	animationObject_->Initialize("simpleSkin");

	//transform_.Initialize();
	//animationTransform_.Initialize();

	//sprite = std::make_unique<Sprite>();
	//sprite->Initialize("Resource/uvChecker.png");
	//sprite->SetSize({ 32.0f,32.0f });

	// ============ライト=================//
	//lightManager_ = std::make_unique<LightManager>();
	

	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightIntensity(0, 1.0f);

	ParticleManager::GetInstance()->CreateParticleGroup("test", "circle.png");

	particleEmitter_ = std::make_unique<ParticleEmitter>();
	particleEmitter_->Initialize("test");

	//grayEffect_ = std::make_unique<GrayEffect>();
	//grayEffect_->Initialize();

	OffScreenManager::GetInstance()->AddEfect(std::make_unique<GrayEffect>());
	OffScreenManager::GetInstance()->AddEfect(std::make_unique<VignetteEffect>());
	OffScreenManager::GetInstance()->AddEfect(std::make_unique<SmoothEffect>());
}

void SampleScene::Finalize()
{
}

void SampleScene::Update()
{
	particleEmitter_->Update({ 0.0f, 0.0f, 0.0f }, 8);
	ParticleManager::GetInstance()->Update();

	object_->AnimationUpdate();
	animationObject_->AnimationUpdate();
	cameraManager_.Update();
	//sprite->Update();

	Vector3 normalCameraPos = normalCamera_->GetTranslate();
	Vector3 cameraRotate = normalCamera_->GetRotate();

	ImGui::Begin("Camera Manager");
	ImGui::DragFloat3("Translate", &normalCameraPos.x, 0.01f);
	ImGui::DragFloat3("Rotate", &cameraRotate.x, 0.01f);
	ImGui::End();

	normalCamera_->SetTranslate(normalCameraPos);
	normalCamera_->SetRotate(cameraRotate);


	DebugUpdate();

	//ImGui::Begin("Transform");
	//ImGui::DragFloat2("UVTranslate0", &static_cast<Model*>(object_->GetModel())->GetMaterials(0)->GetUVData().position.x, 0.01f, -10.0f, 10.0f);
	//ImGui::DragFloat2("UVScale0", &static_cast<Model*>(object_->GetModel())->GetMaterials(0)->GetUVData().size.x, 0.01f, -10.0f, 10.0f);
	//ImGui::SliderAngle("UVRotate0", &static_cast<Model*>(object_->GetModel())->GetMaterials(0)->GetUVData().rotation);
	//ImGui::DragFloat2("UVTranslate1", &static_cast<Model*>(object_->GetModel())->GetMaterials(1)->GetUVData().position.x, 0.01f, -10.0f, 10.0f);
	//ImGui::DragFloat2("UVScale1", &static_cast<Model*>(object_->GetModel())->GetMaterials(1)->GetUVData().size.x, 0.01f, -10.0f, 10.0f);
	//ImGui::SliderAngle("UVRotate1", &static_cast<Model*>(object_->GetModel())->GetMaterials(1)->GetUVData().rotation);
	//ImGui::DragFloat2("UVTranslate2", &static_cast<Model*>(object_->GetModel())->GetMaterials(2)->GetUVData().position.x, 0.01f, -10.0f, 10.0f);
	//ImGui::DragFloat2("UVScale2", &static_cast<Model*>(object_->GetModel())->GetMaterials(2)->GetUVData().size.x, 0.01f, -10.0f, 10.0f);
	//ImGui::SliderAngle("UVRotate2", &static_cast<Model*>(object_->GetModel())->GetMaterials(2)->GetUVData().rotation);
	//ImGui::End();

	object_->Update();

	animationObject_->Update();

	//Vector2 spritePos = sprite->GetPosition();
	//Vector2 spriteSize = sprite->GetSize();
	//float spriteRotate = sprite->GetRotation();

	//Vector2 uvSpritePos = sprite->GetUVPosition();
	//Vector2 uvSpriteSize = sprite->GetUVSize();
	//float uvSpriteRotate = sprite->GetUVRotation();

	//ImGui::Begin("Sprite");
	//ImGui::DragFloat2("position", &spritePos.x);
	//ImGui::DragFloat("rotate", &spriteRotate);
	//ImGui::DragFloat2("size", &spriteSize.x);
	//ImGui::DragFloat2("UVTranslate", &uvSpritePos.x, 0.01f, -10.0f, 10.0f);
	//ImGui::DragFloat2("UVScale", &uvSpriteSize.x, 0.01f, -10.0f, 10.0f);
	//ImGui::SliderAngle("UVRotate", &uvSpriteRotate);
	//ImGui::End();

	//sprite->SetPosition(spritePos);
	//sprite->SetSize(spriteSize);
	//sprite->SetRotation(spriteRotate);

	//sprite->SetUVPosition(uvSpritePos);
	//sprite->SetUVSize(uvSpriteSize);
	//sprite->SetUVRotation(uvSpriteRotate);
}

void SampleScene::Draw()
{
	//ParticleManager::GetInstance()->DrawSet();
	//ParticleManager::GetInstance()->Draw();

	//// 3Dオブジェクト描画前処理
	//Object3dManager::GetInstance()->DrawSetForAnimation();
	//lightManager_->BindLightsToShader();
	//animationObject_->Draw();

	//Object3dManager::GetInstance()->DrawSet();
	//lightManager_->BindLightsToShader();
	//object_->Draw();
  
	// 3Dオブジェクト描画前処理
	Object3dManager::GetInstance()->DrawSetForAnimation();
	lightManager_->BindLightsToShader();
	animationObject_->Draw();

	Object3dManager::GetInstance()->DrawSet();
	lightManager_->BindLightsToShader();
	object_->Draw();
	

	//SpriteManager::GetInstance()->DrawSet();
	//sprite->Draw();

	//PrimitiveDrawer::GetInstance()->DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f, 5.0f, 0.0f }, {1.0f, 1.0f, 1.0f, 1.0f});
	PrimitiveDrawer::GetInstance()->DrawRing({ 0.0f, 0.0f, 0.0f }, 0.2f, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}, 32, RingDrawMode::Fill, "uvChecker.png");
	//PrimitiveDrawer::GetInstance()->DrawRing({ 0.0f, 0.0f, 0.0f }, 0.1f, 0.3f, {1.0f, 1.0f, 1.0f, 1.0f}, 32, RingDrawMode::Line);
}

void SampleScene::DrawRTV()
{


}

#ifdef _DEBUG
void SampleScene::DebugUpdate()
{
	ImGui::Begin("Object");
	object_->DebugGui();
	ImGui::End();

	ImGui::Begin("AnimationObject");
	animationObject_->DebugGui();
	ImGui::End();
}
#endif
