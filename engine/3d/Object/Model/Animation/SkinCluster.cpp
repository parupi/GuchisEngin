#include "SkinCluster.h"
#include "Model.h"
#include <Skeleton.h>

void SkinCluster::Initialize(const SkeletonData& skeleton, Model* model)
{
	skinCluster_ = CreateSkinCluster(skeleton, model);
}

SkinClusterData SkinCluster::CreateSkinCluster(const SkeletonData& skeleton, Model* model)
{
	SkinClusterData skinCluster;

	// palette用のResource確保
	skinCluster.paletteResource = model->GetDxManager()->CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() };
	uint32_t srvIndex = model->GetSrvManager()->Allocate();
	skinCluster.paletteSrvHandle.first = model->GetSrvManager()->GetCPUDescriptorHandle(srvIndex);
	skinCluster.paletteSrvHandle.second = model->GetSrvManager()->GetGPUDescriptorHandle(srvIndex);

	//palette用のSRVを生成
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	model->GetDxManager()->GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);

	// influence用のResourceを確保
	skinCluster.influenceResource = model->GetDxManager()->CreateBufferResource(sizeof(VertexInfluence) * model->GetModelData().vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * model->GetModelData().vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence, model->GetModelData().vertices.size() };

	// Influence用のVBVを生成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * model->GetModelData().vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	// InverseBindPoseMatrixを格納する場所を探し、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);

	for (const auto& jointWeight : model->GetModelData().skinClusterData) { // ModelのSkinClusterの情報を解析
		auto it = skeleton.jointMap.find(jointWeight.first); // JointWeight.firstはJoint名
		if (it == skeleton.jointMap.end()) { // 見つからなかったら次に回す
			continue;
		}
		// (*it).secondにはjointのindexが入っている
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}

void SkinCluster::UpdateSkinCluster(const SkeletonData& skeleton)
{
	// スキニング行列を更新
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Transpose(Inverse(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}
