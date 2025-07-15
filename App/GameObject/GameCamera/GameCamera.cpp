#include "GameCamera.h"
#include <3d/Object/Object3dManager.h>


GameCamera::GameCamera(std::string objectName) : Camera(objectName)
{
	GetTranslate() = { 0.0f, 8.0f, -20.0f };
	GetRotate() = { 0.5f, 0.0f, 0.0f };
}

void GameCamera::Update()
{

	Camera::Update();
}
