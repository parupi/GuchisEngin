#include "Camera.h"
#include "imgui.h"

Camera::Camera(const std::string cameraName)
	: transform_({ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} })
	, horizontalFOV_(0.45f)
	, aspectRatio_(float(WindowManager::kClientWidth) / float(WindowManager::kClientHeight))
	, nearClip_(0.1f)
	, farClip_(200.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_))
{
	cameraName_ = cameraName;
	global_->LoadFiles();
	global_->CreateGroup(cameraName_);
	global_->AddItem(cameraName_, "Translate", Vector3{});
	global_->AddItem(cameraName_, "Rotate", Vector3{});
}

void Camera::Update()
{
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_);
	worldViewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void Camera::FollowCamera(const Vector3& target)
{
	// カメラの位置を対象の後方に設定
	ImGui::Begin("Camera Manager");
	ImGui::DragFloat3("normalPos", &followCameraOffsetPosition_.x, 0.01f);
	ImGui::DragFloat3("normalRotate", &followCameraOffsetRotare_.x, 0.01f);
	ImGui::End();
	transform_.rotate = followCameraOffsetRotare_;
	transform_.translate = target + followCameraOffsetPosition_; // 例: 後方10ユニット、上に4ユニットオフセット
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);

}

Vector2 Camera::WorldToScreen(const Vector3& worldPos, int screenWidth, int screenHeight) const
{
	// 1. ワールド座標をビュー空間に変換
	Vector4 clipPos = Vector4(worldPos.x, worldPos.y, worldPos.z, 1.0f) * viewMatrix_ * projectionMatrix_;

	// 2. NDCに変換 (透視投影のため、wで割る)
	if (clipPos.w != 0.0f) {
		clipPos.x /= clipPos.w;
		clipPos.y /= clipPos.w;
		clipPos.z /= clipPos.w;
	}

	// 3. スクリーン座標に変換
	Vector2 screenPos;
	screenPos.x = (clipPos.x * 0.5f + 0.5f) * screenWidth;
	screenPos.y = (1.0f - (clipPos.y * 0.5f + 0.5f)) * screenHeight; // Y軸を反転

	return screenPos;
}