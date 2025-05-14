#include "SkinnedModel.h"
#include "Model/Animation/Skeleton.h"
#include "Model/Animation/SkinCluster.h"
#include "Model/Animation/Animation.h"
#include <TextureManager.h>


void SkinnedModel::Initialize(ModelLoader* modelLoader, const std::string& directoryPath, const std::string& fileName)
{
	// モデルローダーの保持
	modelLoader_ = modelLoader;
	directoryPath_ = directoryPath;

	// モデルの読み込み
	modelData_ = modelLoader_->LoadSkinnedModel(directoryPath, fileName);

	// スケルトン作成
	skeleton_ = std::make_unique<Skeleton>();
	skeleton_->Initialize(this);

	// アニメーション作成
	animator_ = std::make_unique<Animation>();
	animator_->Initialize(this, fileName);

	// メッシュとマテリアルの作成
	for (auto& skinnedMeshData : modelData_.meshes) {
		auto mesh = std::make_unique<Mesh>();
		mesh->Initialize(GetDxManager(), GetSrvManager(), skinnedMeshData.meshData);
		meshes_.emplace_back(std::move(mesh));
	}

	for (auto& materialData : modelData_.materials) {
		auto material = std::make_unique<Material>();
		material->Initialize(GetDxManager(), GetSrvManager(), materialData);
		materials_.emplace_back(std::move(material));
	}



	// スキンクラスタ作成
	skinCluster_ = std::make_unique<SkinCluster>();
	const auto& skinClusterData = modelData_.skinClusterData;
	const auto& meshData = modelData_.meshes.at(0); // 複数メッシュがある場合は要拡張

	skinCluster_->Initialize(skeleton_->GetSkeletonData(), meshData, skinClusterData, GetDxManager(), GetSrvManager());


	//// 各メッシュをMeshクラスへ変換
	//for (const auto& meshData : modelData_.meshes) {
	//	auto mesh = std::make_unique<Mesh>();

	//	const auto& skinCluster = modelData_.skinClusterData.at(meshData.skinClusterName);

	//	const auto& skinClusterData = modelData_.skinClusterData.at(meshData.skinClusterName);
	//	skinCluster_->Initialize(skeleton_.get(), meshData, skinClusterData, modelLoader_->GetDxManager(), modelLoader_->GetSrvManager());

	//	mesh->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), meshData.meshData);
	//	meshes_.push_back(std::move(mesh));
	//}

	//// 各メッシュをMeshクラスへ変換
	//for (const auto& materialData : modelData_.materials) {
	//	auto material = std::make_unique<Material>();
	//	material->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), materialData);
	//	materials_.push_back(std::move(material));
	//}


}

void SkinnedModel::Update()
{
	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, animator_->GetAnimation().duration); // 最後までいったらリピート再生

	skeleton_->ApplyAnimation(animator_->GetAnimation(), animationTime);
	skeleton_->Update();
	skinCluster_->UpdateSkinCluster(skeleton_->GetSkeletonData());
}

void SkinnedModel::Draw()
{



	//modelLoader_->GetDxManager()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	// wvp用のCBufferの場所を設定
	//modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform->GetConstBuffer()->GetGPUVirtualAddress());




	modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootDescriptorTable(13, skinCluster_->GetSkinCluster().paletteSrvHandle.second);


	const auto& view = skinCluster_->GetSkinCluster().influenceBufferView;
	modelLoader_->GetDxManager()->GetCommandList()->IASetVertexBuffers(1, 1, &view);
	// SRVのDescriptorTableの先頭を設定。
	//modelLoader_->GetSrvManager()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath));
	// ドローコール
	//modelLoader_->GetDxManager()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_..indices.size()), 1, 0, 0, 0);


	for (size_t i = 0; i < materials_.size(); i++) {
		materials_[i]->Draw();
	}

	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i]->Draw();
	}
}
