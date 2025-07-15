#include "ParticleManager.h"
#include "math/function.h"
#include <numbers>
#include <debuger/ImGuiManager.h>
#include <algorithm> 
#include <base/utility/DeltaTime.h>
#include <base/TextureManager.h>

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleManager();
	}
	return instance;
}

void ParticleManager::Finalize()
{
	// パーティクルグループのインスタンシングリソースを解放
	for (auto& [name_, group] : particleGroups_) {
		group.instancingResource.Reset();
		group.instancingDataPtr = nullptr;
		group.particleList.clear();
	}
	particleGroups_.clear();

	// パーティクル用のリソース
	instancingResource_.Reset();
	materialResource_.Reset();
	vertexResource.Reset();

	instancingData_ = nullptr;
	materialData_ = nullptr;
	vertexData_ = nullptr;

	// パーティクルパラメータ・α値もクリア
	particleParams_.clear();
	alpha_.clear();

	// 全体のパーティクルリストもクリア
	particles.clear();

	// カメラへの参照も解放（ただし所有権はない）
	camera_ = nullptr;

	// 外部への参照も切っておく（所有権は持っていないのでdeleteはしない）
	dxManager_ = nullptr;
	srvManager_ = nullptr;
	psoManager_ = nullptr;

	delete instance;
	instance = nullptr;

	// ログ出力など（任意）
	Logger::Log("ParticleManager finalized.\n");
}

void ParticleManager::Initialize(DirectXManager* dxManager, SrvManager* srvManager, PSOManager* psoManager)
{
	dxManager_ = dxManager;
	srvManager_ = srvManager;
	psoManager_ = psoManager;

	// リソースの生成と値の設定
	CreateParticleResource();
	CreateMaterialResource();

	// jsonファイルの読み込み
	global_->LoadFiles();
}


void ParticleManager::Update()
{
	if (camera_ == nullptr) return;

	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	Matrix4x4 viewProjectionMatrix = viewMatrix * projectionMatrix;

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * cameraMatrix;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	for (auto& [groupName, particleGroup] : particleGroups_) {
		numInstance = 0;

		for (auto particleIterator = particleGroup.particleList.begin(); particleIterator != particleGroup.particleList.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				// パーティクルが生存時間を超えたら削除
				particleIterator = particleGroup.particleList.erase(particleIterator);
				continue;
			}

			if (numInstance >= kNumMaxInstance) {
				break;
			}

			// パーティクルの更新処理
			float alpha{};
			isBillboard = global_->GetValueRef<bool>(groupName, "IsBillboard");

			(*particleIterator).transform.translate += (*particleIterator).velocity * DeltaTime::GetDeltaTime();
			(*particleIterator).currentTime += DeltaTime::GetDeltaTime();
			alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			// ワールド行列の計算
			scaleMatrix = MakeScaleMatrix((*particleIterator).transform.scale);
			translateMatrix = MakeTranslateMatrix((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix{};
			if (isBillboard) {
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;
			} else {
				worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale, (*particleIterator).transform.rotate, (*particleIterator).transform.translate);
			}
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

			// インスタンシングデータの設定
			if (numInstance < kNumMaxInstance) {
				instancingData_[numInstance].WVP = worldViewProjectionMatrix;
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].color = (*particleIterator).color;
				instancingData_[numInstance].color.w = alpha;
				++numInstance;
			}

			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instanceCount = numInstance;

		// GPU メモリにインスタンスデータを書き込む
		if (particleGroup.instancingDataPtr) {
			std::memcpy(particleGroup.instancingDataPtr, instancingData_, sizeof(ParticleForGPU) * numInstance);
		}

		particleParams_[groupName] = LoadParticleParameters(global_, groupName);
	}
}

void ParticleManager::Draw()
{
	// グラフィックスパイプラインの設定
	auto commandList = dxManager_->GetCommandList();
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);



	// すべてのパーティクルグループを描画
	for (auto& [groupName, particleGroup] : particleGroups_) {
		if (particleGroup.instanceCount > 0) {
			// マテリアルCBufferの場所を指定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
			// SRVのDescriptorTableを設定
			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvIndex);
			// テクスチャのSRVのDescriptorTableを設定
			   // テクスチャのSRVのDescriptorTableを設定
			srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.materialData.textureFilePath));

			commandList->DrawInstanced(6, particleGroup.instanceCount, 0, 0);
		}
	}
}

void ParticleManager::CreateParticleGroup(const std::string name_, const std::string textureFilePath)
{
	if (particleGroups_.contains(name_)) {
		// 登録済みの名前なら早期リターン
		return;
	}

	// グループを追加
	particleGroups_[name_] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups_[name_];

	// テクスチャを読み込む（未読み込みならロードする）
	particleGroup.materialData.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(particleGroup.materialData.textureFilePath);
	particleGroup.materialData.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

	// インスタンシング用リソースの生成
	particleGroup.instancingResource = dxManager_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	// インスタンシング用にSRVを確保してSRVインデックスを記録
	//particleGroup.srvIndex = srvManager_->Allocate();

	// インスタンシングデータを書き込むためのポインタを取得
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingDataPtr));

	// SRVを作成するDescriptorの場所を決める
	particleGroup.srvIndex = srvManager_->Allocate() + 1;

	// SRVの生成
	srvManager_->CreateSRVforStructuredBuffer(particleGroup.srvIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	// インスタンス数を初期化
	particleGroup.instanceCount = 0;

	// パーティクルグループごとにグローバルバリアースを作る
	global_->AddItem(name_, "minTranslate", Vector3{});
	global_->AddItem(name_, "maxTranslate", Vector3{});

	global_->AddItem(name_, "minRotate", Vector3{});
	global_->AddItem(name_, "maxRotate", Vector3{});

	global_->AddItem(name_, "minScale", Vector3{});
	global_->AddItem(name_, "maxScale", Vector3{});

	global_->AddItem(name_, "minVelocity", Vector3{});
	global_->AddItem(name_, "maxVelocity", Vector3{});

	global_->AddItem(name_, "minLifeTime", float{});
	global_->AddItem(name_, "maxLifeTime", float{});

	global_->AddItem(name_, "minColor", Vector3{});
	global_->AddItem(name_, "maxColor", Vector3{});

	global_->AddItem(name_, "minAlpha", float{});
	global_->AddItem(name_, "maxAlpha", float{});

	global_->AddItem(name_, "IsBillboard", bool{});
}

void ParticleManager::DrawSet(BlendMode blendMode)
{
	dxManager_->GetCommandList()->SetPipelineState(psoManager_->GetParticlePSO(blendMode));
	dxManager_->GetCommandList()->SetGraphicsRootSignature(psoManager_->GetParticleSignature());
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


#ifdef _DEBUG
void ParticleManager::DebugGui()
{

}
#endif // DEBUG

void ParticleManager::CreateParticleResource()
{
	// インスタンス用のTransformationMatrixリソースを作る
	instancingResource_ = dxManager_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// 単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {

		instancingData_[index].WVP = MakeIdentity4x4();

		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// モデルの読み込み
	ModelData modelData;
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	//modelData.material.textureFilePath = "resource/uvChecker.png";
	// 頂点リソースを作る
	vertexResource = dxManager_->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	vertexBufferView_.BufferLocation = vertexResource->GetGPUVirtualAddress();	// リソースの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());		// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);	// 1頂点当たりのサイズ

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));		// 書き込むためのアドレスを取得
	std::memcpy(vertexData_, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void ParticleManager::CreateMaterialResource()
{
	// マテリアル用のリソースを作る。
	materialResource_ = dxManager_->CreateBufferResource(sizeof(Material));
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 白を入れる
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
}

ParticleManager::Particle ParticleManager::MakeNewParticle(const std::string name_/*, std::mt19937& randomEngine*/, const Vector3& translate)
{
	Particle particle{};

	auto& params = particleParams_[name_];

	auto FixRange = [](const Vector2& range) {
		auto [minVal, maxVal] = std::minmax(range.x, range.y);
		return Vector2{ minVal, maxVal };
		};

	auto FixRangeVec3 = [](const Vector3& minVal, const Vector3& maxVal) {
		return std::pair<Vector3, Vector3>{
			{(std::min)(minVal.x, maxVal.x), (std::min)(minVal.y, maxVal.y), (std::min)(minVal.z, maxVal.z)},
			{ (std::max)(minVal.x, maxVal.x), (std::max)(minVal.y, maxVal.y), (std::max)(minVal.z, maxVal.z) }
		};
		};

	// 各範囲を修正
	params.translateX = FixRange(params.translateX);
	params.translateY = FixRange(params.translateY);
	params.translateZ = FixRange(params.translateZ);

	params.rotateX = FixRange(params.rotateX);
	params.rotateY = FixRange(params.rotateY);
	params.rotateZ = FixRange(params.rotateZ);

	params.scaleX = FixRange(params.scaleX);
	params.scaleY = FixRange(params.scaleY);
	params.scaleZ = FixRange(params.scaleZ);

	params.velocityX = FixRange(params.velocityX);
	params.velocityY = FixRange(params.velocityY);
	params.velocityZ = FixRange(params.velocityZ);

	params.lifeTime = FixRange(params.lifeTime);

	auto [colorMin, colorMax] = FixRangeVec3(params.colorMin, params.colorMax);
	params.colorMin = colorMin;
	params.colorMax = colorMax;

	std::uniform_real_distribution<float> distTranslationX(params.translateX.x, params.translateX.y);
	std::uniform_real_distribution<float> distTranslationY(params.translateY.x, params.translateY.y);
	std::uniform_real_distribution<float> distTranslationZ(params.translateZ.x, params.translateZ.y);

	std::uniform_real_distribution<float> distRotationX(params.rotateX.x, params.rotateX.y);
	std::uniform_real_distribution<float> distRotationY(params.rotateY.x, params.rotateY.y);
	std::uniform_real_distribution<float> distRotationZ(params.rotateZ.x, params.rotateZ.y);

	std::uniform_real_distribution<float> distScaleX(params.scaleX.x, params.scaleX.y);
	std::uniform_real_distribution<float> distScaleY(params.scaleY.x, params.scaleY.y);
	std::uniform_real_distribution<float> distScaleZ(params.scaleZ.x, params.scaleZ.y);

	std::uniform_real_distribution<float> distVelocityX(params.velocityX.x, params.velocityX.y);
	std::uniform_real_distribution<float> distVelocityY(params.velocityY.x, params.velocityY.y);
	std::uniform_real_distribution<float> distVelocityZ(params.velocityZ.x, params.velocityZ.y);

	std::uniform_real_distribution<float> distTime(params.lifeTime.x, params.lifeTime.y);
	std::uniform_real_distribution<float> distColorR(params.colorMin.x, params.colorMax.x);
	std::uniform_real_distribution<float> distColorG(params.colorMin.y, params.colorMax.y);
	std::uniform_real_distribution<float> distColorB(params.colorMin.z, params.colorMax.z);
	particle.transform.scale = { distScaleX(randomEngine), distScaleY(randomEngine), distScaleZ(randomEngine) };
	particle.transform.rotate = { distRotationX(randomEngine), distRotationY(randomEngine), distRotationZ(randomEngine) };
	Vector3 randomTranslate = { distTranslationX(randomEngine), distTranslationY(randomEngine), distTranslationZ(randomEngine) };
	particle.transform.translate = translate + randomTranslate;
	particle.velocity = { distVelocityX(randomEngine), distVelocityY(randomEngine), distVelocityZ(randomEngine) };
	particle.color = { distColorR(randomEngine) , distColorG(randomEngine) , distColorB(randomEngine) , 1.0f };
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0.0f;

	return particle;
}

ParticleManager::ParticleParameters ParticleManager::LoadParticleParameters(GlobalVariables* global, const std::string& groupName)
{
	ParticleParameters params{};

	// Translate
	params.translateX = { global->GetValueRef<Vector3>(groupName, "minTranslate").x, global->GetValueRef<Vector3>(groupName, "maxTranslate").x };
	params.translateY = { global->GetValueRef<Vector3>(groupName, "minTranslate").y, global->GetValueRef<Vector3>(groupName, "maxTranslate").y };
	params.translateZ = { global->GetValueRef<Vector3>(groupName, "minTranslate").z, global->GetValueRef<Vector3>(groupName, "maxTranslate").z };

	// GetValue<Vector3>
	params.rotateX = { global->GetValueRef<Vector3>(groupName, "minRotate").x, global->GetValueRef<Vector3>(groupName, "maxRotate").x };
	params.rotateY = { global->GetValueRef<Vector3>(groupName, "minRotate").y, global->GetValueRef<Vector3>(groupName, "maxRotate").y };
	params.rotateZ = { global->GetValueRef<Vector3>(groupName, "minRotate").z, global->GetValueRef<Vector3>(groupName, "maxRotate").z };

	// Scale
	params.scaleX = { global->GetValueRef<Vector3>(groupName, "minScale").x, global->GetValueRef<Vector3>(groupName, "maxScale").x };
	params.scaleY = { global->GetValueRef<Vector3>(groupName, "minScale").y, global->GetValueRef<Vector3>(groupName, "maxScale").y };
	params.scaleZ = { global->GetValueRef<Vector3>(groupName, "minScale").z, global->GetValueRef<Vector3>(groupName, "maxScale").z };

	// Velocity
	params.velocityX = { global->GetValueRef<Vector3>(groupName, "minVelocity").x, global->GetValueRef<Vector3>(groupName, "maxVelocity").x };
	params.velocityY = { global->GetValueRef<Vector3>(groupName, "minVelocity").y, global->GetValueRef<Vector3>(groupName, "maxVelocity").y };
	params.velocityZ = { global->GetValueRef<Vector3>(groupName, "minVelocity").z, global->GetValueRef<Vector3>(groupName, "maxVelocity").z };

	// LifeTime
	params.lifeTime = { global->GetValueRef<float>(groupName, "minLifeTime"), global->GetValueRef<float>(groupName, "maxLifeTime") };

	// Color
	params.colorMin = global->GetValueRef<Vector3>(groupName, "minColor");
	params.colorMax = global->GetValueRef<Vector3>(groupName, "maxColor");

	return params;
}

std::list<ParticleManager::Particle> ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count)
{
	ParticleGroup& particleGroup = particleGroups_[name];
	std::list<Particle> newParticles;
	for (uint32_t nowCount = 0; nowCount < count; ++nowCount) {
		Particle particle = MakeNewParticle(name, position);
		newParticles.push_back(particle);
	}
	particleGroup.particleList.splice(particleGroup.particleList.end(), newParticles);
	return newParticles;
}