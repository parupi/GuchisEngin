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
    Quaternion rotation = MakeRotateAxisAngleQuaternion(Normalize(Vector3{ 1.0f, 0.4f, -0.2f }), 0.45);
    Vector3 pointY = { 2.1f, -0.9f, 1.3f };
    Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
    Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
    Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

    PrintOnImGui(rotation, "rotation");
    PrintOnImGui(rotateMatrix, "rotateMatrix");
    PrintOnImGui(rotateByQuaternion), "rotateByQuaternion";
    PrintOnImGui(rotateByMatrix, "rotateByMatrix");
}

void GameScene::Draw()
{
}