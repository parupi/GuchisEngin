#pragma once
#include "WindowManager.h"
#include "DirectXManager.h"
#include "SrvManager.h"
#include "Input.h"
#ifdef _DEBUG
#include "LeakChecker.h"
#endif // _DEBUG

#include <SceneManager.h>
#include <SceneFactory.h>
#include <AbstractSceneFactory.h>
#include <Audio.h>

class GuchisFramework
{
public:
	virtual ~GuchisFramework() = default;

public:
	// 初期化
	virtual void Initialize();
	// 終了
	virtual void Finalize();
	// 毎フレーム更新
	virtual void Update();
	// 描画
	virtual void Draw() = 0;
	// 終了チェック
	virtual bool IsEndRequest() { return winManager->ProcessMessage(); }

public:
	// 実行
	void Run();

protected:
#ifdef _DEBUG
	std::unique_ptr<D3DResourceLeakChecker> leakCheck;
#endif // _DEBUG

	std::unique_ptr<WindowManager> winManager = nullptr;
	std::unique_ptr<DirectXManager> dxManager = nullptr;
	std::unique_ptr<SrvManager> srvManager = nullptr;
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
};

