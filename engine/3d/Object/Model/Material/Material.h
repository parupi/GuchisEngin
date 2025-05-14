#pragma once
#include <Model/ModelStructs.h>

class SrvManager;
class DirectXManager;

class Material
{
public:
	Material();
	~Material() = default;
	// 初期化
	void Initialize(DirectXManager* directXManager, SrvManager* srvManager, MaterialData materialData);
	// 描画
	void Draw();

private:
	void CreateMaterialResource();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	MaterialForGPU* materialForGPU_ = nullptr;
	MaterialData materialData_;

	DirectXManager* directXManager_ = nullptr;
	SrvManager* srvManager_ = nullptr;

	

	EulerTransform uvTransform_{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Vector2 uvPosition_ = { 0.0f, 0.0f };
	float uvRotation_ = 0.0f;
	Vector2 uvSize_ = { 1.0f, 1.0f };
public:
	// 色
	const Vector4& GetColor() const { return materialForGPU_->color; }
	void SetColor(const Vector4& color) { materialForGPU_->color = color; }
	// Lighting
	bool GetIsLighting() const { return materialForGPU_->enableLighting; }
	void SetIsLighting(const bool isLighting) { materialForGPU_->enableLighting = isLighting; }
};

