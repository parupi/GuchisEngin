#include "Object3d.h"
#include "Object3dManager.h"
#include "TextureManager.h"
//#include "imgui.h"
#include <WorldTransform.h>
#include <numbers>
//#include <Skeleton.h>
//#include <Animator.h>
#include "Model/ModelManager.h"
Object3d::~Object3d()
{
}

void Object3d::Initialize(const std::string& fileName)
{
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(fileName);

	objectManager_ = Object3dManager::GetInstance();

	transform_ = std::make_unique<WorldTransform>();
	transform_->Initialize();

	CreateCameraResource();
}

void Object3d::AnimationUpdate()
{
	//if (model_->GetModelData().isAnimation) {
	//	model_->Update();
	//}
}

void Object3d::Update()
{
	transform_->TransferMatrix();



	camera_ = objectManager_->GetDefaultCamera();
	cameraData_->worldPosition = camera_->GetTranslate();

	Matrix4x4 worldViewProjectionMatrix;
	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = transform_->GetMatWorld() * viewProjectionMatrix;
	} else {
		worldViewProjectionMatrix = transform_->GetMatWorld();
	}

	//if (model_->GetModelData().isAnimation) {
		//if (model_->GetModelData().isHasBones) {
		//	transform_->SetMapWVP(worldViewProjectionMatrix);
		//	transform_->SetMapWorld(transform_->GetMatWorld());
		//} else {
		//	transform_->SetMapWVP(model_->GetModelData().rootNode.localMatrix * worldViewProjectionMatrix);
		//	transform_->SetMapWorld(model_->GetModelData().rootNode.localMatrix * transform_->GetMatWorld());
		//}
	//} else {
		transform_->SetMapWVP(worldViewProjectionMatrix);
		transform_->SetMapWorld(transform_->GetMatWorld());
	//}
}

void Object3d::Draw()
{
	// cameraの場所を指定
	objectManager_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	objectManager_->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform_->GetConstBuffer()->GetGPUVirtualAddress());
	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}


#ifdef _DEBUG
void Object3d::DebugGui()
{
	if (ImGui::TreeNode("Models")) {
		auto& modelMap = ModelManager::GetInstance()->models;
		static std::vector<std::string> modelNames;
		static int selectedIndex = 0;

		// モデル一覧を初期化（必要なら一度だけでOK）
		if (modelNames.empty()) {
			for (const auto& pair : modelMap) {
				modelNames.push_back(pair.first);
			}
		}

		if (!modelNames.empty()) {
			const char* currentItem = modelNames[selectedIndex].c_str();

			if (ImGui::BeginCombo("Model List", currentItem)) {
				for (int i = 0; i < modelNames.size(); ++i) {
					bool isSelected = (selectedIndex == i);
					if (ImGui::Selectable(modelNames[i].c_str(), isSelected)) {
						selectedIndex = i;
						SetModel(modelNames[selectedIndex]);
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::TreePop();
	}

	transform_->DebugGui();
}
#endif // _DEBUG

void Object3d::CreateCameraResource()
{
	// カメラ用のリソースを作る
	cameraResource_ = objectManager_->GetDxManager()->CreateBufferResource(sizeof(CameraForGPU));
	// 書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	// 初期値を入れる
	cameraData_->worldPosition = { 1.0f, 1.0f, 1.0f };
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}
