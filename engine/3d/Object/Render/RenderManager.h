#pragma once
#include <mutex>
#include <DirectXManager.h>
#include "BaseRender.h"
class RenderManager
{
private:
	static RenderManager* instance;
	static std::once_flag initInstanceFlag;

	RenderManager() = default;
	~RenderManager() = default;
	RenderManager(RenderManager&) = default;
	RenderManager& operator=(RenderManager&) = default;
public:
	// インスタンスの取得
	static RenderManager* GetInstance();
	// 初期化処理
	void Initialize(DirectXManager* dxManager);

	// レンダー追加処理
	void AddRender(std::unique_ptr<BaseRender> render);

	BaseRender* FindRender(std::string renderName);

	DirectXManager* GetDxManager() { return dxManager_; }
private:
	DirectXManager* dxManager_ = nullptr;

	std::vector<std::unique_ptr<BaseRender>> renders_;
};

