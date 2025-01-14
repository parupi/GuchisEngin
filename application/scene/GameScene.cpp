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
    Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
    Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

    Quaternion identity = Identity();
    Quaternion conj = Conjugate(q1);
    Quaternion inv = Inverse(q1);
    Quaternion normal = Normalize(q1);
    Quaternion mul1 = q1 * q2;
    Quaternion mul2 = q2 * q1;
    float norm = Norm(q1);

    PrintOnImGui(identity, "Identity");
    PrintOnImGui(conj, "Conjugate");
    PrintOnImGui(inv), "Inverse";
    PrintOnImGui(normal, "Normalize");
    PrintOnImGui(mul1, "Multiply(q1, q2)");
    PrintOnImGui(mul2, "Multiply(q2, q1)");
    PrintOnImGui(norm, "Norm");
}

void GameScene::Draw()
{
}