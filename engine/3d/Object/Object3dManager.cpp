#include "Object3dManager.h"
#include "3d/Object/Object3d.h"
#include <cassert>
#include <3d/Light/LightManager.h>
#include <3d/Camera/CameraManager.h>

Object3dManager* Object3dManager::instance = nullptr;
std::once_flag Object3dManager::initInstanceFlag;

Object3dManager* Object3dManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = new Object3dManager();
		});
	return instance;
}

void Object3dManager::Initialize(DirectXManager* directXManager, PSOManager* psoManager)
{
	assert(directXManager);
	dxManager_ = directXManager;
	psoManager_ = psoManager;
}

void Object3dManager::Finalize()
{
	objects_.clear();

	dxManager_ = nullptr;
	psoManager_ = nullptr;
	defaultCamera_ = nullptr;

	delete instance;
	instance = nullptr;
}

void Object3dManager::Update()
{
	for (auto& object : objects_) {
		object->Update();
	}
}

void Object3dManager::DrawObject()
{
	for (auto& object : objects_) {
		// ブレンドモードが違っていたら新しくPSOを設定
		if (blendMode_ != object->GetOption().blendMode) {
			// ブレンドモードを最新にしておく
			blendMode_ = object->GetOption().blendMode;

			dxManager_->GetCommandList()->SetPipelineState(psoManager_->GetObjectPSO(blendMode_));			// PSOを設定
			dxManager_->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetObjectSignature());
			dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// psoのセットをしたらlight,cameraもバインドしておく
			LightManager::GetInstance()->BindLightsToShader();
			CameraManager::GetInstance()->BindCameraToShader();
		}

		object->Draw();
	}


	// 次回の為にNoneに戻しておく
	blendMode_ = BlendMode::kNone;
}

void Object3dManager::DrawSetForAnimation()
{
	dxManager_->GetCommandList()->SetPipelineState(psoManager_->GetAnimationPSO());			// PSOを設定
	dxManager_->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetAnimationSignature());
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3dManager::AddObject(std::unique_ptr<Object3d> object)
{
	objects_.push_back(std::move(object));
}

Object3d* Object3dManager::FindObject(std::string objectName)
{
	for (auto& object : objects_) {
		if (object->name_ == objectName) {
			return object.get();
		}
	}
	Logger::Log("not found renderer");
	return nullptr;
}

