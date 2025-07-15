#include "Player.h"
#include <3d/Object/Renderer/RendererManager.h>
#include <input/Input.h>
#include <base/utility/DeltaTime.h>

Player::Player(std::string objectName) : Object3d(objectName)
{
}

void Player::Initialize()
{

	Object3d::Initialize();
}

void Player::Update()
{

	Object3d::Update();
}

void Player::Draw()
{
	Object3d::Draw();
}
