#include "Material.h"
#include "DirectXManager.h"
#include "SrvManager.h"
#include <TextureManager.h>

Material::Material()
{

}

void Material::Initialize(DirectXManager* directXManager, SrvManager* srvManager, MaterialData materialData)
{
	directXManager_ = directXManager;
	srvManager_ = srvManager;
	materialData_ = materialData;

	CreateMaterialResource();

	// .objの参照しているテクスチャファイルの読み込み
	//TextureManager::GetInstance()->LoadTexture(materialData_.textureFilePath);
	// 読み込んだテクスチャの番号を取得
	//materialData_.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(materialData_.textureFilePath);
}

void Material::Draw()
{
	// uvTransformに値を適用
	uvTransform_.translate = { uvPosition_.x, uvPosition_.y, 0.0f };
	uvTransform_.rotate = { 0.0f, 0.0f, uvRotation_ };
	uvTransform_.scale = { uvSize_.x, uvSize_.y, 1.0f };
	// Transform情報を作る
	Matrix4x4 uvTransformMatrix = MakeIdentity4x4();
	uvTransformMatrix *= MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix *= MakeRotateZMatrix(uvTransform_.rotate.z);
	uvTransformMatrix *= MakeTranslateMatrix(uvTransform_.translate);
	materialForGPU_->uvTransform = uvTransformMatrix;

	// マテリアルCBufferの場所を指定
	directXManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	srvManager_->SetGraphicsRootDescriptorTable(2, materialData_.textureIndex);
}

void Material::CreateMaterialResource()
{
	// マテリアル用のリソースを作る。今回はFcolor1つ分のサイズを用意する
	materialResource_ = directXManager_->CreateBufferResource(sizeof(MaterialForGPU));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialForGPU_));
	// 白を入れる
	materialForGPU_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialForGPU_->enableLighting = true;
	materialForGPU_->uvTransform = MakeIdentity4x4();
	materialForGPU_->shininess = 20.0f;

}
