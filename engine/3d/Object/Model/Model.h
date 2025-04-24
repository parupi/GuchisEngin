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
#include "SkinCluster.h"
#include "Skeleton.h"
#include "Animation/AnimationStructs.h"
//#include <DebugSphere.h>

class WorldTransform;
class Animator;
//class Skeleton;
//class SkinCluster;

class Model
{
public: // メンバ関数
	// 初期化
	void Initialize(ModelLoader* modelLoader, const std::string& directoryPath, const std::string& fileName);
	// 描画
	void Draw(WorldTransform* transform);
private:
	//// 頂点データの生成
	//void CreateVertexResource();
	//// Indexデータの生成
	//void CreateIndexResource();

public: // 構造体

	// 現在のアニメーションタイム
	float animationTime = 0.0f;
	///↑↑↑アニメーション用構造体(引っ越し予定)↑↑↑///
private:
	ModelLoader* modelLoader_ = nullptr;
	//Skeleton* skeleton_ = nullptr;

	ModelData modelData_;
	std::unique_ptr<Animator> animator_;
	std::unique_ptr<Skeleton> skeleton_;
	std::unique_ptr<SkinCluster> skinCluster_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	std::string directoryPath_;

	// デバッグ用変数
	//std::vector<DebugSphere> spheres_;
public:
	// mtlファイルを読む関数
	//static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	// モデルを読む関数
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	// ノードをモデルデータに変換する関数
	static Node ReadNode(aiNode* node);
	// ボーンの保持を確認する
	static bool HasBones(const aiScene* scene);

	/////↓↓↓アニメーション用関数(引っ越し予定)↓↓↓///
	////// アニメーションを読む処理
	////Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);
	////// アニメーションの更新
	////void UpdateAnimation();
	//// 骨骨の更新
	//void UpdateSkeleton(SkeletonData& skeleton);
	////// 任意の時刻を取得する (Vector3)
	////Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	////// 任意の時刻を取得する (Quaternion)
	////Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
	//// Nodeの階層からSkeletonを作る
	//SkeletonData CreateSkeleton(const Model::Node& rootNode);
	//// NodeからJointを作る
	//int32_t CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	//// アニメーションを適用する
	//void ApplyAnimation(SkeletonData& skeleton, const Animator::Animation& animation, float animationTime);
	//// 全体の更新
	void Update();
	//// SkinClusterを生成する関数
	//SkinCluster CreateSkinCluster(const SkeletonData& skeleton, const ModelData& modelData);
	//// スキンクラスターの更新
	//void UpdateSkinCluster(SkinCluster& skinCluster, const SkeletonData& skeleton);
	/////↑↑↑アニメーション用関数(引っ越し予定)↑↑↑///


	/////↓↓↓デバッグ用関数↓↓↓///
	//std::vector<Vector3> GetConnectionPositions();
	//uint32_t GetConnectionCount();
	/////↑↑↑デバッグ用関数↑↑↑///

public: // ゲッター // セッター //
	//void SetVertices(VertexData vertex);
	void SetTexturePath(const std::string& filePath) { modelData_.material.textureFilePath = filePath; }

	ModelData GetModelData() { return modelData_; }
	std::string GetDirectoryPath() { return directoryPath_; }
	DirectXManager* GetDxManager() { return modelLoader_->GetDxManager(); }
	SrvManager* GetSrvManager() { return modelLoader_->GetSrvManager(); }
	Animator* GetAnimator() { return animator_.get(); }
	void SetLocalMatrix(const Matrix4x4& matrix) { modelData_.rootNode.localMatrix = matrix; }
	//SkinCluster GetSkinCluster(){return }
	//void SetSkeleton(Skeleton* skeleton) { skeleton_ = skeleton; }
	//Skeleton* GetSkeleton() { return skeleton_; }

};

