#pragma once

#include "AbstractSceneFactory.h"

// このゲーム用のシーン工場
class SceneFactory : public AbstractSceneFactory
{
public:
	// 新しいシーンを生成
	BaseScene* CreateScene(const std::string& sceneName) override;

};

