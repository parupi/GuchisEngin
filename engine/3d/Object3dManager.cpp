#include "Object3dManager.h"
#include <PsoManager.h>

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
	delete instance;
	instance = nullptr;
}

void Object3dManager::DrawSet()
{
	dxManager_->GetCommandList()->SetPipelineState(psoManager_->GetObjectSpritePSO().Get()); // PSOを設定
	dxManager_->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetObjectSignature().Get());
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3dManager::DrawSetForAnimation()
{
	dxManager_->GetCommandList()->SetPipelineState(psoManager_->GetAnimationSpritePSO().Get()); // PSOを設定
	dxManager_->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetAnimationSignature().Get());
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
