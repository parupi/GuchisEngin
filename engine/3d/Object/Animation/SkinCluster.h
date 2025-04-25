#pragma once
#include "AnimationStructs.h"

class Model;
class Skeleton;

class SkinCluster
{
private:



public:
	void Initialize(const SkeletonData& skeleton, Model* model);

	// SkinClusterを生成する関数
	SkinClusterData CreateSkinCluster(const SkeletonData& skeleton, Model* model);
	// スキンクラスターの更新
	void UpdateSkinCluster(const SkeletonData& skeleton);

private:
	// スキンクラスター
	SkinClusterData skinCluster_;

public:
	SkinClusterData GetSkinCluster() const { return skinCluster_; }
};

