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
    Vector3 from0 = Normalize(Vector3{ 1.0f, 0.7f, 0.5f });
    Vector3 to0 = from0 * -1.0f;

    Vector3 from1 = Normalize(Vector3{ -0.6f, 0.9f, 0.2f });
    Vector3 to1 = Normalize(Vector3{ 0.4f, 0.7f, -0.5f });

    Matrix4x4 rotateMatrix0 = DirectionToDirection(
        Normalize(Vector3{ 1.0f, 0.0f, 0.0f }), Normalize(Vector3{ -1.0f, 0.0f, 0.0f })
    );
    Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);
    Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);

    PrintOnImGui(rotateMatrix0);
    PrintOnImGui(rotateMatrix1);
    PrintOnImGui(rotateMatrix2);
}

void GameScene::Draw()
{
}