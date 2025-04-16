#pragma once
#include "AnimationStructs.h"
#include "Animator.h"

class Model;

class Skeleton
{
public:
	// 初期化
	void Initialize(Model* model);
	// 更新
	void Update();
	// SkinClusterの更新
	//void Update();
	// 
	void ApplyAnimation(const Animator::Animation& animation, float animationTime);
private:
	// Nodeの階層からSkeletonを作る
	void CreateSkeleton(const Node& rootNode);
	// NodeからJointを作る
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

private:
	// スケルトンデータ
	SkeletonData skeletonData_;

	// モデルデータ
	Model* model_;

public: // ゲッター // セッター //
	SkeletonData GetSkeletonData() const { return skeletonData_; }
	std::vector<std::pair<std::string, Matrix4x4>> GetBoneMatrices() const;
};

