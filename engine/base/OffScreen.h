#pragma once
#include "DirectXManager.h"
#include <mutex>
#include "PSOManager.h"
class OffScreen
{
public:
	// 初期化
	void Initialize(DirectXManager* dxManager, PSOManager* psoManager);
	// 終了
	void Finalize();
	// 更新処理
	void Update();
	// 描画
	void Draw(/*OffScreenEffectType effectType = OffScreenEffectType::kNone*/);

private:
	DirectXManager* dxManager_ = nullptr;
	PSOManager* psoManager_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	OffScreenEffectType effectType_ = OffScreenEffectType::kNone;
};

