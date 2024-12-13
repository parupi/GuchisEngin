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

void Object3dManager::CreateRootSignature()
{

}

void Object3dManager::CreateRootSignatureForAnimation()
{
	
}

void Object3dManager::CreateInputElementDesc()
{
	
}

void Object3dManager::CreateInputElementDescForAnimation()
{

}

void Object3dManager::CreateBlendState()
{

}

void Object3dManager::CreateRasterizerState()
{

}

void Object3dManager::LoadShader()
{
	// Shaderをコンパイルする
	vertexShaderBlob_ = dxManager_->CompileShader(L"./resource/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxManager_->CompileShader(L"./resource/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void Object3dManager::LoadShaderForAnimation()
{
	// Shaderをコンパイルする
	vertexShaderBlobForAnimation_ = dxManager_->CompileShader(L"./resource/shaders/SkinningObject3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlobForAnimation_ != nullptr);

	pixelShaderBlob_ = dxManager_->CompileShader(L"./resource/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void Object3dManager::CreatePipelineState()
{

}

void Object3dManager::CreatePipelineStateForAnimation()
{
	HRESULT hr;

	// PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureForAnimation_.Get();			// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDescForAnimation_;			// InputLayouty
	graphicsPipelineStateDesc.VS = { vertexShaderBlobForAnimation_->GetBufferPointer(), vertexShaderBlobForAnimation_->GetBufferSize() };			// vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };			// PixelShade
	graphicsPipelineStateDesc.BlendState = blendDesc_;					// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;			// RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	hr = dxManager_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateForAnimation_));
	assert(SUCCEEDED(hr));
}
