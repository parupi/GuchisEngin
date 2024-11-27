#include "ParticleManager.h"
#include <function.h>
#include <TextureManager.h>
#include <numbers>
#include <imgui.h>

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());

void ParticleManager::Finalize()
{

}

void ParticleManager::Initialize()
{
	dxManager_ = ParticleResources::GetInstance()->GetDxManager();
	srvManager_ = ParticleResources::GetInstance()->GetSrvManager();
	camera_ = ParticleResources::GetInstance()->GetCamera();

	// リソースの生成と値の設定
	CreateParticleResource();
	CreateMaterialResource();
}


void ParticleManager::Update()
{
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
		uint32_t numInstance = 0;

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
			float alpha;
			if (groupName == "snow") {
				if ((*particleIterator).transform.translate.y <= 0) {
					(*particleIterator).currentTime += kDeltaTime;
					alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
				}
				else {
					(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;
					alpha = 1.0f;
				}
			}
			else {
				if ((*particleIterator).isAlive) {
					(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;
					alpha = 1.0f;
				}
				else {
					(*particleIterator).currentTime += kDeltaTime;
					alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
				}
			}

			// ワールド行列の計算
			scaleMatrix = MakeScaleMatrix((*particleIterator).transform.scale);
			translateMatrix = MakeTranslateMatrix((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix{};
			if (isBillboard) {
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;
			}
			else {
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

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath)
{
	if (particleGroups_.contains(name)) {
		// 登録済みの名前なら早期リターン
		return;
	}

	// グループを追加
	particleGroups_[name] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups_[name];

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
}

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

ParticleManager::Particle ParticleManager::MakeNewParticle(const std::string name, std::mt19937& randomEngine, const Vector3& translate)
{
	Particle particle{};
	if (name == "snow") {
		std::uniform_real_distribution<float> distTranslate(-15.0f, 15.0f);
		std::uniform_real_distribution<float> distSize(0.075f, 0.15f);
		std::uniform_real_distribution<float> distVelocity(-0.5f, 0.5f);
		std::uniform_real_distribution<float> distVelocityY(-1.0f, 0.001f);
		std::uniform_real_distribution<float> distColor(0.8f, 1.0f);
		std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

		float scale = distSize(randomEngine);
		particle.transform.scale = { scale, scale, scale };
		particle.transform.rotate = { 0.0f, 0.0f, 0.0f };
		Vector3 randomTranslate = { distTranslate(randomEngine), 20.0f, distTranslate(randomEngine) };
		particle.transform.translate = translate + randomTranslate;
		particle.velocity = { distVelocity(randomEngine), distVelocityY(randomEngine), distVelocity(randomEngine) };
		particle.color = { distColor(randomEngine) , distColor(randomEngine) , distColor(randomEngine) , 1.0f };
		particle.lifeTime = distTime(randomEngine);
		particle.currentTime = 0.0f;
	}
	else {

		std::uniform_real_distribution<float> distTranslationX(particleTranslateX_[name].x, particleTranslateX_[name].y);
		std::uniform_real_distribution<float> distTranslationY(particleTranslateY_[name].x, particleTranslateY_[name].y);
		std::uniform_real_distribution<float> distTranslationZ(particleTranslateZ_[name].x, particleTranslateZ_[name].y);
		std::uniform_real_distribution<float> distRotationX(particleRotateX_[name].x, particleRotateX_[name].y);
		std::uniform_real_distribution<float> distRotationY(particleRotateY_[name].x, particleRotateY_[name].y);
		std::uniform_real_distribution<float> distRotationZ(particleRotateZ_[name].x, particleRotateZ_[name].y);
		std::uniform_real_distribution<float> distScale(particleScale_[name].x, particleScale_[name].y);
		std::uniform_real_distribution<float> distVelocityX(particleVelocityX_[name].x, particleVelocityX_[name].y);
		std::uniform_real_distribution<float> distVelocityY(particleVelocityY_[name].x, particleVelocityY_[name].y);
		std::uniform_real_distribution<float> distVelocityZ(particleVelocityZ_[name].x, particleVelocityZ_[name].y);
		std::uniform_real_distribution<float> distColor(particleColor_[name].x, particleColor_[name].y);
		std::uniform_real_distribution<float> distTime(particleLifeTime_[name].x, particleLifeTime_[name].y);
		float randomScale = distScale(randomEngine);
		particle.transform.scale = { randomScale, randomScale, randomScale };
		particle.transform.rotate = { distRotationX(randomEngine), distRotationY(randomEngine), distRotationZ(randomEngine) };
		Vector3 randomTranslate = { distTranslationX(randomEngine), distTranslationY(randomEngine), distTranslationZ(randomEngine) };
		particle.transform.translate = translate + randomTranslate;
		particle.velocity = { distVelocityX(randomEngine), distVelocityY(randomEngine), distVelocityZ(randomEngine) };
		particle.color = { distColor(randomEngine) , distColor(randomEngine) , distColor(randomEngine) , 1.0f };
		particle.lifeTime = distTime(randomEngine);
		particle.currentTime = 0.0f;
	}

	return particle;
}

std::list<ParticleManager::Particle> ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count)
{
	ParticleGroup& particleGroup = particleGroups_[name];
	std::list<Particle> newParticles;
	for (uint32_t nowCount = 0; nowCount < count; ++nowCount) {
		Particle particle = MakeNewParticle(name, randomEngine, position);
		newParticles.push_back(particle);
	}
	particleGroup.particleList.splice(particleGroup.particleList.end(), newParticles);
	return newParticles;
}