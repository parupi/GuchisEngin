#pragma once
#include <cstdint>
#include <array>
#include <Matrix4x4.h>
#include <vector>
#include <d3d12.h>
#include <span>
#include <wrl.h>
#include <optional>
#include <string>
#include <map>
#include "function.h"
#include "Vector2.h"
#include <Vector4.h>

// 骨
struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

struct SkeletonData {
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};

// スキンクラスター
static const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix; // 位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

struct SkinClusterData {
	std::vector<Matrix4x4> inverseBindPoseMatrices;

	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;

	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	uint32_t srvIndex;
};

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