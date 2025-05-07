#include "SkinnedModel.h"

void SkinnedModel::Initialize(ModelLoader* modelLoader, const std::string& directoryPath, const std::string& fileName)
{
	modelLoader_ = modelLoader;

	directoryPath_ = directoryPath;

	// モデルの読み込み
	modelData_ = modelLoader_->LoadSkinnedModel(directoryPath_, fileName);

	// 各メッシュをMeshクラスへ変換
	for (const auto& meshData : modelData_.meshes) {
		auto mesh = std::make_unique<Mesh>();

		const auto& skinCluster = modelData_.skinClusterData.at(meshData.skinClusterName);
		std::vector<VertexInfluence> influences = GenerateInfluences(skinCluster, meshData..size());


		mesh->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), meshData);
		meshes_.push_back(std::move(mesh));
	}

	// 各メッシュをMeshクラスへ変換
	for (const auto& materialData : modelData_.materials) {
		auto material = std::make_unique<Material>();
		material->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), materialData);
		materials_.push_back(std::move(material));
	}
}

void SkinnedModel::Update()
{
}

void SkinnedModel::Draw()
{
}
