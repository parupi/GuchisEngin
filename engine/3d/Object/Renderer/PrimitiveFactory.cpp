#include "PrimitiveFactory.h"
#include "MeshGenerator.h"
#include <TextureManager.h>

// PrimitiveFactory.cpp
std::unique_ptr<Model> PrimitiveFactory::Create(PrimitiveRenderer::PrimitiveType type) {
    MeshData meshData;
    MaterialData mat;
    switch (type) {
    case PrimitiveRenderer::PrimitiveType::Plane:
        meshData = MeshGenerator::CreatePlane();
        mat.name = "PlaneMaterial";
        break;
    case PrimitiveRenderer::PrimitiveType::Ring:
        meshData = MeshGenerator::CreateRing();
        mat.name = "RingMaterial";
        break;
    case PrimitiveRenderer::PrimitiveType::Cylinder:
        meshData = MeshGenerator::CreateCylinder();
        mat.name = "CylinderMaterial";
        break;
    }

    meshData.materialIndex = 0;
 
  
    mat.textureFilePath = "uvChecker.png";
    TextureManager::GetInstance()->LoadTexture("uvChecker.png");
    mat.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath("uvChecker.png");

    auto model = std::make_unique<Model>();
    model->InitializeFromMesh(meshData, mat); // モデルにメッシュを直接渡す初期化関数が必要
    return model;
}