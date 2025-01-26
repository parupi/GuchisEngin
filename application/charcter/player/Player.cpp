#include "Player.h"
#include "IdleState.h"

void Player::Initialize() {
	GlobalInitialize("firstAttack");
	GlobalInitialize("secondAttack");
	GlobalInitialize("thirdAttack");
	GlobalInitialize("fourthAttack");
	GlobalInitialize("fifthAttack");
	GlobalInitialize("sixthAttack");

	object_ = std::make_unique<Object3d>();
	object_->Initialize("models/player/player.obj");

	transform_.Initialize();
	transform_.translation_.y += 1.0f;
	transform_.scale_ = { 0.5f, 0.5f, 0.5f };

	shadeObject_ = std::make_unique<Object3d>();
	shadeObject_->Initialize("models/player/player.obj");
	shadeObject_->SetColor({ 0.0f, 0.0f, 0.0f, 0.9f });
	shadeObject_->SetIsLighting(false);

	shadeTransform_.Initialize();
	shadeTransform_.scale_ = { 0.5f, 0.01f, 0.5f };

	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize();
	weapon_->SetParent(transform_);

	dustEmitter_ = std::make_unique<ParticleEmitter>();
	dustEmitter_->Initialize("Dust");
	dustEmitter_->SetFrequency(0.025f);

	input_ = Input::GetInstance();
	currentState_ = std::make_unique<IdleState>();
	currentState_->Enter(this);

	// jsonファイルの読み込み
	global_->LoadFiles();
}

void Player::Finalize() {
	currentState_->Exit(this);
}

void Player::Update() {
	isChangeState_ = false;
	ApplyGravity();
	currentState_->Update(this);
	transform_.TransferMatrix();
	shadeTransform_.translation_ = transform_.translation_;
	shadeTransform_.translation_.y = 0.0f;
	shadeTransform_.TransferMatrix();

	weapon_->Update();

	dustEmitter_->SetActiveFlag(isDustParticle);
	dustEmitter_->Update(transform_.translation_, 5);

}

void Player::Draw() {
	Object3dManager::GetInstance()->DrawSet();
	object_->Draw(transform_);
	weapon_->Draw();
	shadeObject_->Draw(shadeTransform_);
}

bool Player::GetIsHit()
{
	bool isHit = weapon_->GetIsHit();
	if (isHit && attackData_.isAttack) {
		return true;
	}
	return false;
}

void Player::ChangeState(std::unique_ptr<PlayerState> newState) {
	isChangeState_ = true;
	currentState_->Exit(this);
	currentState_ = std::move(newState);
	currentState_->Enter(this);
}

Vector3 Player::GetCameraForward() const {
	float yaw = camera_->GetRotate().y;
	return Vector3(std::sin(yaw), 0.0f, std::cos(yaw));
}

Vector3 Player::GetCameraRight() const {
	float yaw = camera_->GetRotate().y;
	return Vector3(std::cos(yaw), 0.0f, -std::sin(yaw));
}

Vector3 Player::GetFrontDirection() const
{
	// 回転行列からZ軸（正面方向）を取得
	return Vector3(transform_.matWorld_.m[2][0], transform_.matWorld_.m[2][1], transform_.matWorld_.m[2][2]); // Z軸方向
}

void Player::GlobalInitialize(const std::string stateName)
{
	global_->AddItem(stateName, "startPos", Vector3{});
	global_->AddItem(stateName, "midPos", Vector3{});
	global_->AddItem(stateName, "endPos", Vector3{});
	global_->AddItem(stateName, "startRot", Vector3{});
	global_->AddItem(stateName, "midRot", Vector3{});
	global_->AddItem(stateName, "endRot", Vector3{});
	global_->AddItem(stateName, "time", float{});
	global_->AddItem(stateName, "KnockBack", Vector3{});
	global_->AddItem(stateName, "playerMove", Vector3{});
}

void Player::ApplyGravity()
{
	vel_ += gravity * deltaTime;

	// 位置を更新
	transform_.translation_ += vel_ * deltaTime;

	// 地面との衝突を考慮
	if (transform_.translation_.y < 0.5f) {
		transform_.translation_.y = 0.5f; // 地面上に固定
		vel_.y = -vel_.y * 0.2f; // 反発係数を適用
		if (fabs(vel_.y) < 0.01f) vel_.y = 0.0f;
	}
}
