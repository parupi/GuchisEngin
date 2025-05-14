#include "Model.h"
#include "function.h"
#include "TextureManager.h"
#include <Vector4.h>
#include <Vector2.h>
#include <Model/ModelManager.h>
#include <WorldTransform.h>
#include <Model/Animation/Animation.h>
#include "Mesh/Mesh.h"
#include "Material/Material.h"
#include <Object3d.h>

void Model::Initialize(ModelLoader* modelManager, const std::string& fileName)
{
	modelLoader_ = modelManager;

	// モデルの読み込み
	modelData_ = modelLoader_->LoadModelFile(fileName);

	// 各メッシュをMeshクラスへ変換
	for (const auto& meshData : modelData_.meshes) {
		auto mesh = std::make_unique<Mesh>();
		mesh->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), meshData);
		meshes_.push_back(std::move(mesh));
	}

	// 各メッシュをMeshクラスへ変換
	for (const auto& materialData : modelData_.materials) {
		auto material = std::make_unique<Material>();
		material->Initialize(modelLoader_->GetDxManager(), modelLoader_->GetSrvManager(), materialData);
		materials_.push_back(std::move(material));
	}
}

void Model::Update()
{
	for (size_t i = 0; i < materials_.size(); i++) {
		materials_[i]->Update();
	}

}

void Model::Draw()
{
		for (size_t i = 0; i < materials_.size(); i++) {
			materials_[i]->Draw();
		}

		for (size_t i = 0; i < meshes_.size(); i++) {
			meshes_[i]->Draw();
		}
}

#ifdef _DEBUG
void Model::DebugGui(Object3d* object)
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
						object->SetModel(modelNames[selectedIndex]);
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

	for (size_t i = 0; i < materials_.size(); i++) {
		if (i == 0) {
			continue;
		}
		materials_[i]->DebugGui(i);
	}
}
#endif // _DEBUG