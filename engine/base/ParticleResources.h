#pragma once
#include <DirectXManager.h>
#include <SrvManager.h>
#include "Camera.h"
#include "PSOManager.h"
class ParticleResources
{
private:
	static ParticleResources* instance;

	ParticleResources() = default;
	~ParticleResources() = default;
	ParticleResources(ParticleResources&) = default;
	ParticleResources& operator=(ParticleResources&) = default;
public:
	// シングルトンインスタンスの取得
	static ParticleResources* GetInstance();
	// 初期化
	void Initialize(DirectXManager* dxManager, SrvManager* srvManager, PSOManager* psoManager);
	// 終了
	void Finalize();
	// 描画前処理
	void DrawSet(BlendMode blendMode = BlendMode::kAdd);
private:
	// DxManager
	DirectXManager* dxManager_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	Camera* camera_ = nullptr;
	PSOManager* psoManager_ = nullptr;

public:
	DirectXManager* GetDxManager() { return dxManager_; }
	SrvManager* GetSrvManager() { return srvManager_; }
	Camera* GetCamera() { return camera_; }
	void SetCamera(Camera* camera) { camera_ = camera; }
};

