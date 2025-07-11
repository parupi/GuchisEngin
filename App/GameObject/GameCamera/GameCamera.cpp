#include "GameCamera.h"
#include <3d/Object/Object3dManager.h>


GameCamera::GameCamera(std::string objectName) : Camera(objectName)
{
	GetTranslate() = { 0.0f, 8.0f, -10.0f };
	GetRotate() = { 0.5f, 0.0f, 0.0f };

	Camera::Update();
}

void GameCamera::Update()
{
	Vector3 playerPos = player_->GetWorldTransform()->GetTranslation();

	// 追従対象の座標を取得・更新
	Vector3& myPos = GetTranslate();

	// x座標は完全に一致させる
	myPos.x = playerPos.x;

	// z座標はプレイヤーより少し後ろ（プレイヤーの向きに応じて調整したければさらに改良可能）
	myPos.z = playerPos.z - 15.0f;


	Camera::Update();
}
