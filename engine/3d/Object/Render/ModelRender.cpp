#include "ModelRender.h"
#include "RenderManager.h"
#include <Model/ModelManager.h>


ModelRender::ModelRender(const std::string& renderName, const std::string& filePath)
{
	localTransform_ = std::make_unique<WorldTransform>();
	localTransform_->Initialize();
	SetModel(filePath);
	name = renderName;
}

void ModelRender::Update(WorldTransform* parentTransform)
{
	model_->Update();

	if (localTransform_->GetParent() == nullptr) {
		localTransform_->SetParent(parentTransform);
	}
	
	localTransform_->TransferMatrix();
	

	Matrix4x4 worldViewProjectionMatrix;
	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = localTransform_->GetMatWorld() * viewProjectionMatrix;
	} else {
		worldViewProjectionMatrix = localTransform_->GetMatWorld();
	}

	localTransform_->SetMapWVP(worldViewProjectionMatrix);
	localTransform_->SetMapWorld(localTransform_->GetMatWorld());
	//localTransform_->SetMapWorld(parentTransform->GetMatWorld());
	//localTransform_->SetMapWVP(parentTransform->GetMatWorld());
}

void ModelRender::Draw(WorldTransform* parentTransform)
{
	RenderManager::GetInstance()->GetDxManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, localTransform_->GetConstBuffer()->GetGPUVirtualAddress());

	model_->Draw();
}

void ModelRender::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

#ifdef _DEBUG
void ModelRender::DebugGui(size_t index)
{
	std::string label = "TransformRender" + std::to_string(index);
	if (ImGui::TreeNode(label.c_str())) {
		localTransform_->DebugGui();
		model_->DebugGui(this);
		ImGui::TreePop();
	}
}
#endif // _DEBUG


