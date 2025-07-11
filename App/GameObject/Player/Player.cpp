#include "Player.h"
#include <3d/Object/Renderer/RendererManager.h>
#include <input/Input.h>
#include <base/utility/DeltaTime.h>

Player::Player(std::string objectName) : Object3d(objectName)
{
}

void Player::Initialize()
{
	// レンダラーを生成
	RendererManager::GetInstance()->AddRenderer(std::make_unique<ModelRenderer>("Player", "Characters_Anne"));
	// レンダラーを追加
	AddRenderer(RendererManager::GetInstance()->FindRender("Player"));




	particleEmitter_ = std::make_unique<ParticleEmitter>();
	particleEmitter_->Initialize("test");
}

void Player::Update()
{
	// 武器を取得
	if (!weapon_) {
		weapon_ = Object3dManager::GetInstance()->FindObject("Weapon");
	}
	// 持つ位置を設定
	Matrix4x4 weaponMatrix = static_cast<SkinnedModel*>(GetRenderer("Player")->GetModel())->GetSkeleton()->GetJointMatrix("Index1.L");

	weaponMatrix *= GetWorldTransform()->GetMatWorld();

	// 1. 分解する
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;
	DecomposeMatrix(weaponMatrix, scale, rotation, translation); // ← 自前関数または既存APIを使う

	// 2. 武器のWorldTransformに適用する
	weapon_->GetWorldTransform()->GetScale() = scale;
	weapon_->GetWorldTransform()->GetRotation() = rotation; // ← Quaternion→Eulerに変換
	weapon_->GetWorldTransform()->GetTranslation() = translation;

	weapon_->Update();

	Matrix4x4 particleMatrix = static_cast<SkinnedModel*>(GetRenderer("Player")->GetModel())->GetSkeleton()->GetJointMatrix("Index1.R");

	// X軸を反転させる（右手系→左手系の補正）

	particleMatrix *= GetWorldTransform()->GetMatWorld();

	particleEmitter_->Update(GetTranslation(particleMatrix), 8);

	float deltaTime = DeltaTime::GetDeltaTime();
	Vector3& pos = GetWorldTransform()->GetTranslation();

	// 接地判定の修正
	bool isGrounded = pos.y <= 0.0f;

	// モデルとアニメーション取得
	SkinnedModel* model = static_cast<SkinnedModel*>(GetRenderer("Player")->GetModel());
	Animation* anim = model->GetAnimation();

	// 移動入力
	// 左スティック移動
	Vector2 leftStick = Input::GetInstance()->GetXInputLeftStick();
	if (leftStick.x >= 0.3f || leftStick.y != 0.0f) {
		// 移動処理
	}

	bool isMoving = false;
	if (Input::GetInstance()->PushKey(DIK_A) || leftStick.x <= -0.3f) {
		pos.x -= 0.1f;
		GetWorldTransform()->GetRotation() = EulerDegree({ 0.0f, -90.0f, 0.0f });
		isMoving = true;
	} else if (Input::GetInstance()->PushKey(DIK_D) || leftStick.x >= 0.3f) {
		pos.x += 0.1f;
		GetWorldTransform()->GetRotation() = EulerDegree({ 0.0f, 90.0f, 0.0f });
		isMoving = true;
	} else if (Input::GetInstance()->PushKey(DIK_S) || leftStick.y < -0.3f){
		pos.z -= 0.1f;
		GetWorldTransform()->GetRotation() = EulerDegree({ 0.0f, 180.0f, 0.0f });
		isMoving = true;
	} else if (Input::GetInstance()->PushKey(DIK_W) || leftStick.y > 0.3f) {
		pos.z += 0.1f;
		GetWorldTransform()->GetRotation() = EulerDegree({ 0.0f, 0.0f, 0.0f });
		isMoving = true;
	}

	// 重力適用
	if (!isGrounded) {
		velocity_.y -= 40.0f * deltaTime;
	} else {
		pos.y = 0.0f; // 地面に固定
		velocity_.y = 0.0f;
	}

	// ジャンプ処理（キー入力によるジャンプ開始）
	if (isGrounded && (Input::GetInstance()->IsXInputAPressed() || Input::GetInstance()->TriggerKey(DIK_SPACE))) {
		velocity_.y = 10.0f;
		if (jumpState_ != JumpState::JumpUp) {
			jumpState_ = JumpState::JumpUp;
			if (anim->GetCurrentAnimationName() != "Jump") {
				anim->Play("Jump", false, 0.5f);
			}
		}
	}

	// ジャンプステートの処理
	switch (jumpState_) {
	case JumpState::JumpUp:
		if (velocity_.y <= 0.0f) {
			jumpState_ = JumpState::JumpIdle;
			if (anim->GetCurrentAnimationName() != "Jump_Idle") {
				anim->Play("Jump_Idle", true, 0.5f);
			}
		}
		break;

	case JumpState::JumpIdle:
		if (isGrounded) {
			jumpState_ = JumpState::JumpLand;
			if (anim->GetCurrentAnimationName() != "Jump_Land") {
				anim->Play("Jump_Land", false, 0.5f);
			}
		}
		break;

	case JumpState::JumpLand:
		//if (anim->IsAnimationFinished("Jump_Land")) {
		jumpState_ = JumpState::None;
		//}
		break;

	case JumpState::None:
		if (!isGrounded) {
			// 地面を外れたらジャンプ（落下）扱いでジャンプ開始
			jumpState_ = JumpState::JumpUp;
			if (anim->GetCurrentAnimationName() != "Jump") {
				anim->Play("Jump", false, 0.5f);
			}
		} else {
			// 地上の通常モーション
			if (isMoving) {
				if (anim->GetCurrentAnimationName() != "Run") {
					anim->Play("Run", true, 0.5f);
				}
			} else {
				if (anim->GetCurrentAnimationName() != "Idle") {
					anim->Play("Idle", true, 0.5f);
				}
			}
		}
		break;
	}


	// 移動
	pos += velocity_ * deltaTime;

	Object3d::Update();
}

void Player::Draw()
{
	Object3d::Draw();
}
