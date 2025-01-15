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
    ImGui::Begin("QuaternionTest");
    ImGui::DragFloat3("pointX", &axis.x, 0.01f);
    ImGui::DragFloat3("pointY", &pointY.x, 0.01f);
    ImGui::DragFloat("angle", &angle, 0.01f);
    ImGui::End();

    Quaternion rotation = MakeRotateAxisAngleQuaternion(Normalize(axis), angle);
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