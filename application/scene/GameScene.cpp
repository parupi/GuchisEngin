#include <GameScene.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <ParticleManager.h>
#include <imgui.h>
#include <Quaternion.h>
#include <Vector3.h>
#include <Matrix4x4.h>

void GameScene::Initialize()
{
}

void GameScene::Finalize()
{
}

void GameScene::Update()
{
    Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
    Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, 3.141592f);

    Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
    Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
    Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
    Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
    Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

    PrintOnImGui(interpolate0, "interpolate0 : ");
    PrintOnImGui(interpolate1, "interpolate1 : ");
    PrintOnImGui(interpolate2, "interpolate2 : ");
    PrintOnImGui(interpolate3, "interpolate3 : ");
    PrintOnImGui(interpolate4, "interpolate4 : ");
}

void GameScene::Draw()
{
}