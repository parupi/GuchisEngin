#pragma once
#pragma once
#include <DirectXManager.h>
#include <SrvManager.h>
#include <random>
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Camera.h>
#include <Vector2.h>
#include "ParticleResources.h"
class ParticleManager
{
public:
	// 終了
	void Finalize();
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	// パーティクルグループを登録する
	void CreateParticleGroup(const std::string name, const std::string textureFilePath);

	// カメラをセットする
	void SetCamera(Camera* camera) { camera_ = camera; }

private: // 構造体

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

	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	struct Particle {
		EulerTransform transform;
		Vector3 velocity;
		Vector3 acc;
		Vector4 color;
		float lifeTime;
		float currentTime;
		bool isAlive;
	};

	struct ParticleGroup {
		MaterialData materialData;  // マテリアルデータ
		std::list<Particle> particleList;  // パーティクルのリスト
		uint32_t srvIndex;  // インスタンシング用SRVインデックス
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;  // インスタンシングリソース
		uint32_t instanceCount;  // インスタンス数
		ParticleForGPU* instancingDataPtr;  // インスタンシングデータを書き込むためのポインタ
	};

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	struct Material {
		Vector4 color;
		bool enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
private:
	// パーティクル用のリソースの生成
	void CreateParticleResource();
	// WVP用のリソースを生成 
	void CreateMaterialResource();
	// パーティクルを生成する関数
	Particle MakeNewParticle(const std::string name, std::mt19937& randomEngine, const Vector3& translate);

public:

	// nameで指定した名前のパーティクルグループにパーティクルを発生させる関数
	std::list<Particle> Emit(const std::string name, const Vector3& position, uint32_t count);

private:
	const uint32_t kNumMaxInstance = 128;	// 最大インスタンス数
	// パーティクル用リソースの宣言
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	ParticleForGPU* instancingData_ = nullptr;
	Material* materialData_;
	VertexData* vertexData_ = nullptr;
private:

	DirectXManager* dxManager_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	Camera* camera_ = nullptr;
	// ランダム用変数宣言
	std::mt19937 randomEngine;

	std::unordered_map<std::string, ParticleGroup> particleGroups_;
	// 汎用化用変数
	std::unordered_map<std::string, Vector2> particleTranslateX_;
	std::unordered_map<std::string, Vector2> particleTranslateY_;
	std::unordered_map<std::string, Vector2> particleTranslateZ_;
	std::unordered_map<std::string, Vector2> particleRotateX_;
	std::unordered_map<std::string, Vector2> particleRotateY_;
	std::unordered_map<std::string, Vector2> particleRotateZ_;
	std::unordered_map<std::string, Vector2> particleScale_;
	std::unordered_map<std::string, Vector2> particleVelocityX_;
	std::unordered_map<std::string, Vector2> particleVelocityY_;
	std::unordered_map<std::string, Vector2> particleVelocityZ_;
	std::unordered_map<std::string, Vector2> particleLifeTime_;
	std::unordered_map<std::string, Vector2> particleColor_;

	bool isBillboard = true;
	const float kDeltaTime = 1.0f / 60.0f;

	Matrix4x4 scaleMatrix;
	Matrix4x4 translateMatrix;


	std::list<Particle> particles;
	uint32_t numInstance = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

public:
	void SetTranslateXLange(const std::string name, float min, float max) { particleTranslateX_[name] = { min, max }; }
	void SetTranslateYLange(const std::string name, float min, float max) { particleTranslateY_[name] = { min, max }; }
	void SetTranslateZLange(const std::string name, float min, float max) { particleTranslateZ_[name] = { min, max }; }
	void SetRotateXLange(const std::string name, float min, float max) { particleRotateX_[name] = { min, max }; }
	void SetRotateYLange(const std::string name, float min, float max) { particleRotateY_[name] = { min, max }; }
	void SetRotateZLange(const std::string name, float min, float max) { particleRotateZ_[name] = { min, max }; }
	void SetScaleLange(const std::string name, float min, float max) { particleScale_[name] = { min, max }; }
	void SetVelocityXLange(const std::string name, float min, float max) { particleVelocityX_[name] = { min, max }; }
	void SetVelocityYLange(const std::string name, float min, float max) { particleVelocityY_[name] = { min, max }; }
	void SetVelocityZLange(const std::string name, float min, float max) { particleVelocityZ_[name] = { min, max }; }
	void SetColorLange(const std::string name, float min, float max) { particleColor_[name] = { min, max }; }
	void SetLifeTimeLange(const std::string name, float min, float max) { particleLifeTime_[name] = { min, max }; }

};
