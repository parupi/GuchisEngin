#pragma once
#include "3d/Camera/Camera.h"
#include "GameObject/Player/Player.h"
class GameCamera : public Camera
{
public: // メンバ関数
	GameCamera(std::string objectName);
	~GameCamera() override = default;

	// 更新
	void Update() override;

	void SetPlayer(Player* player) { player_ = player; }

private:
	Player* player_ = nullptr;

};

