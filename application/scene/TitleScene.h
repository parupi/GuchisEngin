#pragma once
#include <Camera.h>
#include <BaseScene.h>
#include "SceneManager.h"
#include <memory>
#include "Sprite.h"
class TitleScene : public BaseScene
{
public:
	// 初期化
	void Initialize() override;
	// 終了
	void Finalize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;

private:
	std::unique_ptr<Sprite> titleUI_;
};

