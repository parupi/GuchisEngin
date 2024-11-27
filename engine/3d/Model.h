#pragma once
#include "ModelLoader.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Camera.h>
#include <span>
#include <map>
//#include <Skeleton.h>
//#include <Animator.h>

class WorldTransform;
class Animator;
class Skeleton;

class Model
{
public: // メンバ関数
	// 初期化
	void Initialize(ModelLoader* modelLoader, const std::string& directoryPath, const std::string& fileName);
	// 描画
	void Draw(WorldTransform& transform);
private:
	// 頂点データの生成
	void CreateVertexResource();
	// Indexデータの生成
	void CreateIndexResource();

public: // 構造体
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Color {
		float r, g, b;
	};

	struct MaterialData {
		std::string name;
		float Ns;
		Color Ka;	// 環境光色
		Color Kd;	// 拡散反射色
		Color Ks;	// 鏡面反射光
		float Ni;
		float d;
		uint32_t illum;
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	struct Node {
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Node rootNode;
		bool isAnimation;
		bool isHasBones;
	};

private:
	ModelLoader* modelLoader_ = nullptr;
	Skeleton* skeleton_ = nullptr;

	ModelData modelData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	std::string directoryPath_;
public:
	// mtlファイルを読む関数
	//static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	// モデルを読む関数
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	// ノードをモデルデータに変換する関数
	static Node ReadNode(aiNode* node);
	// ボーンの保持を確認する
	static bool HasBones(const aiScene* scene);

public: // ゲッター // セッター //
	void SetVertices(VertexData vertex);
	void SetTexturePath(const std::string& filePath) { modelData_.material.textureFilePath = filePath; }

	ModelData GetModelData() { return modelData_; }
	std::string GetDirectoryPath() { return directoryPath_; }
	DirectXManager* GetDxManager() { return modelLoader_->GetDxManager(); }
	SrvManager* GetSrvManager() { return modelLoader_->GetSrvManager(); }
	//SkinCluster GetSkinCluster(){return }
	void SetSkeleton(Skeleton* skeleton) { skeleton_ = skeleton; }
	Skeleton* GetSkeleton() { return skeleton_; }
};

