#pragma once
#include "ModelLoader.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Camera.h>
#include <span>
#include <map>
#include "Animation/SkinCluster.h"
#include "Animation/Skeleton.h"
#include "ModelStructs.h"
#include "Mesh/Mesh.h"
#include "Material/Material.h"
#include "BaseModel.h"

class Object3d;
class ModelRenderer;
class Model : public BaseModel
{
public: // メンバ関数
	// 初期化
	void Initialize(ModelLoader* modelLoader, const std::string& fileName);

	void InitializeFromMesh(const MeshData& meshData, const MaterialData& materialData);

	void Update() override;
	// 描画
	void Draw() override;
#ifdef _DEBUG
	void DebugGui(ModelRenderer* render);
	void DebugGuiPrimitive();
#endif // _DEBUG


private:
	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::vector<std::unique_ptr<Material>> materials_;

	ModelLoader* modelLoader_ = nullptr;

	ModelData modelData_;
public:
	ModelData GetModelData() { return modelData_; }
	DirectXManager* GetDxManager() { return modelLoader_->GetDxManager(); }
	SrvManager* GetSrvManager() { return modelLoader_->GetSrvManager(); }

	Material* GetMaterials(uint32_t index) {return materials_[index].get();}
};

