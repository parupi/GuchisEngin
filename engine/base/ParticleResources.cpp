#include "ParticleResources.h"

ParticleResources* ParticleResources::instance = nullptr;

ParticleResources* ParticleResources::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleResources();
	}
	return instance;
}

void ParticleResources::Initialize(DirectXManager* dxManager, SrvManager* srvManager, PSOManager* psoManager)
{
	dxManager_ = dxManager;
	srvManager_ = srvManager;
	psoManager_ = psoManager;
}

void ParticleResources::Finalize()
{
	delete instance;
	instance = nullptr;
}

void ParticleResources::DrawSet(BlendMode blendMode)
{
	dxManager_->GetCommandList()->SetPipelineState(psoManager_->GetParticlePSO(blendMode).Get());
	dxManager_->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetParticleSignature().Get());
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}