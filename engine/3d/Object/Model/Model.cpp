#include "Model.h"
#include "function.h"
#include "TextureManager.h"
#include <Vector4.h>
#include <Vector2.h>
#include <Model/ModelManager.h>
#include <WorldTransform.h>
#include <Model/Animation/Animation.h>
#include "Mesh/Mesh.h"
#include "Material/Material.h"

void Model::Initialize(ModelLoader* modelManager, const std::string& directoryPath, const std::string& fileName)
{
	modelLoader_ = modelManager;

	directoryPath_ = directoryPath;

	// モデルの読み込み
	modelData_ = modelLoader_->LoadModelFile(directoryPath_, fileName);

	// 各メッシュをMeshクラスへ変換
	for (const auto& meshData : modelData_.meshes) {
		auto mesh = std::make_unique<Mesh>();
		mesh->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), meshData);
		meshes_.push_back(std::move(mesh));
	}

	// 各メッシュをMeshクラスへ変換
	for (const auto& materialData : modelData_.materials) {
		auto material = std::make_unique<Material>();
		material->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), materialData);
		materials_.push_back(std::move(material));
	}

	//// アニメーションがあれば
	//if (modelData_.isAnimation) {
	//	animator_ = std::make_unique<Animation>();
	//	animator_->Initialize(this, fileName);
	//	// モデルがボーンを持っていれば
	//	if (modelData_.isHasBones) {
	//		skeleton_ = std::make_unique<Skeleton>();
	//		skeleton_->Initialize(this);
	//		skinCluster_ = std::make_unique<SkinCluster>();
	//		skinCluster_->Initialize(skeleton_->GetSkeletonData(), this);
	//	}
	//}
}

void Model::Draw()
{
	//if (modelData_.isHasBones) {

		//D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		//	vertexBufferView_,
		//	skinCluster_->GetSkinCluster().influenceBufferView
		//};

		//// VertexBufferViewを設定
		//modelLoader_->GetDxManager()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
		//modelLoader_->GetDxManager()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
		//// wvp用のCBufferの場所を設定
		////modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform->GetConstBuffer()->GetGPUVirtualAddress());
		//modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootDescriptorTable(13, skinCluster_->GetSkinCluster().paletteSrvHandle.second);
		//// SRVのDescriptorTableの先頭を設定。
		//modelLoader_->GetSrvManager()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath));
		//// ドローコール
		//modelLoader_->GetDxManager()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
	//} else {
		// wvp用のCBufferの場所を設定
		//modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform->GetConstBuffer()->GetGPUVirtualAddress());

		

		// ドローコール
		//modelLoader_->GetDxManager()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

		for (size_t i = 0; i < materials_.size(); i++) {
			materials_[i]->Draw();
		}

		for (size_t i = 0; i < meshes_.size(); i++) {
			meshes_[i]->Draw();
		}

	//}
}

//void Model::CreateVertexResource()
//{
//	// 頂点リソースを作る
//	vertexResource_ = modelLoader_->GetDxManager()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
//	// 頂点バッファビューを作成する
//	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();	// リソースの先頭アドレスから使う
//	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
//	vertexBufferView_.StrideInBytes = sizeof(VertexData); // 1頂点当たりのサイズ
//	// 頂点リソースにデータを書き込む
//	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_)); // 書き込むためのアドレスを取得
//	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
//}
//
//void Model::CreateIndexResource()
//{
//	indexResource_ = modelLoader_->GetDxManager()->CreateBufferResource(sizeof(uint32_t) * modelData_.indices.size());
//
//	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
//	indexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * modelData_.indices.size());
//	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
//
//	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
//	std::memcpy(indexData_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
//}

//ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
//	ModelData modelData;
//
//	Assimp::Importer importer;
//	std::string filePath = directoryPath + "/" + filename;
//
//	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
//	assert(scene && scene->HasMeshes());
//
//	modelData.rootNode = ReadNode(scene->mRootNode);
//	modelData.isAnimation = (scene->mNumAnimations > 0);
//	modelData.isHasBones = HasBones(scene);
//
//	// --- マテリアルの読み込み ---
//	modelData.materials.resize(scene->mNumMaterials);
//	for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
//		aiMaterial* material = scene->mMaterials[i];
//		MaterialData matData;
//		matData.name = material->GetName().C_Str();
//
//		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
//			aiString textureFilePath;
//			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
//			matData.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
//			TextureManager::GetInstance()->LoadTexture(matData.textureFilePath);
//			matData.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(matData.textureFilePath);
//		}
//
//		modelData.materials[i] = matData;
//	}
//
//	// --- メッシュの読み込み ---
//	modelData.meshes.resize(scene->mNumMeshes);
//	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
//		MeshData meshData;
//		aiMesh* mesh = scene->mMeshes[meshIndex];
//
//		assert(mesh->HasNormals());
//		assert(mesh->HasTextureCoords(0));
//
//		meshData.vertices.resize(mesh->mNumVertices);
//		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
//			aiVector3D& pos = mesh->mVertices[vertexIndex];
//			aiVector3D& norm = mesh->mNormals[vertexIndex];
//			aiVector3D& uv = mesh->mTextureCoords[0][vertexIndex];
//
//			meshData.vertices[vertexIndex].position = { -pos.x, pos.y, pos.z, 1.0f };
//			meshData.vertices[vertexIndex].normal = { -norm.x, norm.y, norm.z };
//			meshData.vertices[vertexIndex].texcoord = { uv.x, uv.y };
//		}
//
//		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
//			aiFace& face = mesh->mFaces[faceIndex];
//			if (face.mNumIndices == 4) {
//				uint32_t i0 = face.mIndices[0];
//				uint32_t i1 = face.mIndices[1];
//				uint32_t i2 = face.mIndices[2];
//				uint32_t i3 = face.mIndices[3];
//				meshData.indices.insert(meshData.indices.end(), { i0, i1, i2, i0, i2, i3 });
//			} else if (face.mNumIndices == 3) {
//				for (uint32_t i = 0; i < 3; ++i) {
//					meshData.indices.push_back(face.mIndices[i]);
//				}
//			}
//		}
//
//		// --- ボーン情報の抽出 ---
//		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
//			aiBone* bone = mesh->mBones[boneIndex];
//			std::string jointName = bone->mName.C_Str();
//			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];
//
//			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
//			aiVector3D scale, translate;
//			aiQuaternion rotate;
//			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
//
//			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
//				{ scale.x, scale.y, scale.z },
//				{ rotate.x, -rotate.y, -rotate.z, rotate.w },
//				{ -translate.x, translate.y, translate.z }
//			);
//			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);
//
//			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
//				jointWeightData.vertexWeights.push_back({
//					bone->mWeights[weightIndex].mWeight,
//					bone->mWeights[weightIndex].mVertexId
//					});
//			}
//		}
//
//		// --- マテリアルとの紐付け ---
//		meshData.materialIndex = mesh->mMaterialIndex;
//
//		modelData.meshes[meshIndex] = meshData;
//	}
//
//	return modelData;
//}

//Node Model::ReadNode(aiNode* node)
//{
//	Node result;
//	aiVector3D scale, translate;
//	aiQuaternion rotate;
//	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数
//	result.transform.scale = { scale.x, scale.y, scale.z }; // scaleはそのまま
//	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; // xを反転、回転方向が逆なので軸も反転
//	result.transform.translate = { -translate.x, translate.y, translate.z }; // x軸を反転
//	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
//
//	result.name = node->mName.C_Str(); // node名を格納
//	result.children.resize(node->mNumChildren); // 子供の数だけ確保
//	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
//		// 再帰的に読むことで階層構造を作る
//		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
//	}
//
//	return result;
//}

//bool Model::HasBones(const aiScene* scene)
//{
//	// メッシュがなければボーンもない
//	if (!scene->HasMeshes()) {
//		return false;
//	}
//
//	// 各メッシュをチェック
//	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
//		aiMesh* mesh = scene->mMeshes[meshIndex];
//
//		// ボーン数が0でなければボーンがあると判断
//		if (mesh->mNumBones > 0) {
//			return true;
//		}
//	}
//
//	// どのメッシュにもボーンが含まれていない場合
//	return false;
//}

void Model::Update()
{
	//animationTime += 1.0f / 60.0f;
	//animationTime = std::fmod(animationTime, animator_->GetAnimation().duration); // 最後までいったらリピート再生

	//if (modelData_.isHasBones) {
	//	skeleton_->ApplyAnimation(animator_->GetAnimation(), animationTime);
	//	skeleton_->Update();
	//	skinCluster_->UpdateSkinCluster(skeleton_->GetSkeletonData());
	//} else {
	//	animator_->Update();
	//}
}
