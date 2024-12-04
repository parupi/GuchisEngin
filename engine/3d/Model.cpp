#include "Model.h"
#include "function.h"
#include "TextureManager.h"
#include <Vector4.h>
#include <Vector2.h>
#include <ModelManager.h>
#include <WorldTransform.h>
//#include <Skeleton.h>
//#include <Animator.h>

void Model::Initialize(ModelLoader* modelManager, const std::string& directoryPath, const std::string& fileName)
{
	modelLoader_ = modelManager;

	directoryPath_ = directoryPath;

	// モデルの読み込み
	modelData_ = LoadModelFile(directoryPath_, fileName);
	animation_ = LoadAnimationFile(directoryPath_, fileName);
	skeleton_ = CreateSkeleton(modelData_.rootNode);

	CreateVertexResource();
	CreateIndexResource();

	// .objの参照しているテクスチャファイルの読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	// 読み込んだテクスチャの番号を取得
	modelData_.material.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
}

void Model::Draw(WorldTransform& transform)
{
	if (modelData_.isHasBones) {

		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vertexBufferView_,
			vertexBufferView_
			//skeleton_->GetSkinCluster().influenceBufferView
		};

		// wvp用のCBufferの場所を設定
		modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform.GetConstBuffer()->GetGPUVirtualAddress());
		//modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootDescriptorTable(13, skeleton_->GetSkinCluster().paletteSrvHandle.second);
		// VertexBufferViewを設定
		//modelLoader_->GetDxManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
		modelLoader_->GetDxManager()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
		modelLoader_->GetDxManager()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
		// SRVのDescriptorTableの先頭を設定。
		modelLoader_->GetSrvManager()->SetGraphicsRootDescriptorTable(2, modelData_.material.textureIndex);
		// ドローコール
		//modelLoader_->GetDxManager()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
		modelLoader_->GetDxManager()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
	}
	else {
		// wvp用のCBufferの場所を設定
		modelLoader_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform.GetConstBuffer()->GetGPUVirtualAddress());
		// VertexBufferViewを設定
		modelLoader_->GetDxManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
		modelLoader_->GetDxManager()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
		// SRVのDescriptorTableの先頭を設定。
		modelLoader_->GetSrvManager()->SetGraphicsRootDescriptorTable(2, modelData_.material.textureIndex);
		// ドローコール
		//modelLoader_->GetDxManager()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
		modelLoader_->GetDxManager()->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
	}
}

void Model::CreateVertexResource()
{
	// 頂点リソースを作る
	vertexResource_ = modelLoader_->GetDxManager()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	// 頂点バッファビューを作成する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();	// リソースの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData); // 1頂点当たりのサイズ
	// 頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_)); // 書き込むためのアドレスを取得
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::CreateIndexResource()
{
	indexResource_ = modelLoader_->GetDxManager()->CreateBufferResource(sizeof(uint32_t) * modelData_.indices.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
}

Model::ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;

	// モデルの生成とファイルの読み込み
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

	// アニメーションが含まれているかを判別
	modelData.isAnimation = (scene->mNumAnimations > 0);
	// ボーンが含まれているかを判別
	modelData.isHasBones = HasBones(scene);

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないメッシュは今回は非対応
		assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回は非対応
		modelData.vertices.resize(mesh->mNumVertices); // 最初に頂点分のメモリを確保しておく

		// Vertexの情報を取得
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			// 右手系から左手系に変換する
			//position *= -1.0f;
			//normal *= -1.0f;
			modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
		}

		// 面からIndexの情報を取得する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみをサポート

			// Indexの解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}

		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			// Jointごとの格納領域を作る
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			// InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix;
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrixFromQuaternion(
				{ scale.x, scale.y, scale.z },
				{ rotate.x, -rotate.y, -rotate.z, rotate.w },
				{ -translate.x, translate.y, translate.z }
			);
			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix;

			// Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	// materialの解析を行う
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

Model::Node Model::ReadNode(aiNode* node)
{
	Node result;

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数
	result.transform.scale = { scale.x, scale.y, scale.z }; // scaleはそのまま
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; // xを反転、回転方向が逆なので軸も反転
	result.transform.translate = { -translate.x, translate.y, translate.z }; // x軸を反転
	result.localMatrix = MakeAffineMatrixFromQuaternion(result.transform.scale, result.transform.rotate, result.transform.translate);

	result.name = node->mName.C_Str(); // node名を格納
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読むことで階層構造を作る
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

bool Model::HasBones(const aiScene* scene)
{
	// メッシュがなければボーンもない
	if (!scene->HasMeshes()) {
		return false;
	}

	// 各メッシュをチェック
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// ボーン数が0でなければボーンがあると判断
		if (mesh->mNumBones > 0) {
			return true;
		}
	}

	// どのメッシュにもボーンが含まれていない場合
	return false;
}

Model::Animation Model::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	//animation; // 今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0); // アニメーションがない場合
	aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用
	animation_.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を変換

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation_.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		// Position
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 秒に変換
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		// Scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 秒に変換
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手変換は不要
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}

		// Rotation
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 秒に変換

			// 右手->左手座標系の変換
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z, -keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
	}
	// 解析完了
	return animation_;
}

void Model::UpdateAnimation()
{
	

	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name]; // ルートノードに入ってるアニメーションを取得
	//NodeAnimation& rootNodeAnimation = animation.nodeAnimations["mixamorig:LeftArm"]; // ルートノードに入ってるアニメーションを取得
	Vector3 scale, translate;
	Quaternion rotate;
	//if (!model_->GetModelData().isHasBones) {
	translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
	rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
	scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
	modelData_.rootNode.localMatrix = MakeAffineMatrixFromQuaternion(scale, rotate, translate);
}

void Model::UpdateSkeleton(SkeletonData& skeleton)
{
	for (auto& joint : skeleton.joints) {
		joint.localMatrix = MakeAffineMatrixFromQuaternion(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		if (joint.parent && *joint.parent < skeleton.joints.size()) {
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

Vector3 Model::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion Model::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

void Model::SetVertices(VertexData vertex)
{
	modelData_.vertices.push_back(vertex);
}

Model::SkeletonData Model::CreateSkeleton(const Model::Node& rootNode) {
	SkeletonData skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}
	return skeleton;
}

int32_t Model::CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = static_cast<int32_t>(joints.size());
	joint.parent = parent;
	joints.push_back(joint);

	for (const auto& child : node.children) {
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

void Model::ApplyAnimation(SkeletonData& skeleton, const Animation& animation, float animationTime)
{
	for (auto& joint : skeleton.joints) {
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& nodeAnimation = it->second;
			joint.transform.translate = CalculateValue(nodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculateValue(nodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(nodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void Model::Update()
{
	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, animation_.duration); // 最後までいったらリピート再生

	//UpdateAnimation();
	ApplyAnimation(skeleton_, animation_, animationTime);
	UpdateSkeleton(skeleton_);

	for (auto& joint : skeleton_.joints) {
		PrintOnImGui(joint.transform.translate, "Skeleton");
		PrintOnImGui(joint.transform.rotate, "Skeleton");
		PrintOnImGui(joint.transform.scale, "Skeleton");
	}
}
