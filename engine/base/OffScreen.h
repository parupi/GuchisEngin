#pragma once
#include "DirectXManager.h"
#include <mutex>
class OffScreen
{
public:
	// 初期化
	void Initialize(DirectXManager* dxManager);
	// 終了
	void Finalize();
	// 描画
	void Draw();
private:
	// ルートシグネチャ
	void CreateRootSignature();
	// pso生成
	void CreatePipelineState();

private:
	DirectXManager* dxManager_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
};

