#pragma once
#include "Object3dManager.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "function.h"
#include <fstream>
#include "Model/Model.h"
#include "Model/BaseModel.h"
#include <Camera.h>
class Object3dManager;
class WorldTransform;

class Object3d
{
public: // メンバ関数
	Object3d() = default;
	virtual ~Object3d();

	virtual void Initialize(const std::string& filePath);
	// アニメーション用アップデート
	virtual void AnimationUpdate();
	virtual void Update();
	virtual void Draw();

#ifdef _DEBUG
	virtual void DebugGui();
#endif // _DEBUG


private:

	void CreateCameraResource();
private: // 構造体


	// カメラ座標
	struct CameraForGPU {
		Vector3 worldPosition;
	};

private: // メンバ変数
	Object3dManager* objectManager_ = nullptr;
	BaseModel* model_ = nullptr;
	//SkinnedModel* skinnedModel_ = nullptr;
	Camera* camera_ = nullptr;
	//Animator* animator_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr;


	CameraForGPU* cameraData_ = nullptr;



	std::unique_ptr<WorldTransform> transform_;
public: // ゲッター // セッター // 
	// モデル
	//void SetModel(Model* model) { model_ = model; }
	void SetModel(const std::string& filePath);
	void SetModel(BaseModel* model) {model_ = model;}
	BaseModel* GetModel() { return model_; }
	//Animator* GetAnimator() { return animator_; }
	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	// uv平行移動
	//const Vector2& GetUVPosition() const { return uvPosition_; }
	//void SetUVPosition(const Vector2& position) { uvPosition_ = position; }
	//// 回転
	//float GetUVRotation() const { return uvRotation_; }
	//void SetUVRotation(float rotation) { uvRotation_ = rotation; }
	//// 拡縮
	//const Vector2& GetUVSize() const { return uvSize_; }
	//void SetUVSize(const Vector2& size) { uvSize_ = size; }
	// ワールドトランスフォームの取得
	WorldTransform* GetWorldTransform() { return transform_.get(); }
};