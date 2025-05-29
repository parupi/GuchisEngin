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
#include <Renderer/BaseRenderer.h>
#include <Collider/BaseCollider.h>
class Object3dManager;
class WorldTransform;

class Object3d
{
public: // メンバ関数
	Object3d() = default;
	virtual ~Object3d();
	// 初期化処理
	virtual void Initialize(const std::string& filePath);
	// 更新処理
	virtual void Update();
	virtual void Draw();

#ifdef _DEBUG
	virtual void DebugGui();
#endif // _DEBUG

	// 衝突した
	virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other);
	// 衝突中
	virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other);
	// 離れた
	virtual void OnCollisionExit([[maybe_unused]] BaseCollider* other);

private: // メンバ変数
	Object3dManager* objectManager_ = nullptr;
	//BaseModel* model_ = nullptr;
	//SkinnedModel* skinnedModel_ = nullptr;
	Camera* camera_ = nullptr;
	//Animator* animator_ = nullptr;

	std::unique_ptr<WorldTransform> transform_;

	std::vector<BaseRenderer*> renders_;
	std::vector<BaseCollider*> colliders_;
public: // ゲッター // セッター // 
	// モデル
	//void SetModel(const std::string& filePath);
	//BaseModel* GetModel() { return model_; }

	// レンダー追加処理
	void AddRenderer(BaseRenderer* render);
	void AddCollider(BaseCollider* collider);


	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	// ワールドトランスフォームの取得
	WorldTransform* GetWorldTransform() { return transform_.get(); }
};