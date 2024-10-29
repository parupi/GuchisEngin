#include "ModelManager.h"

ModelManager* ModelManager::instance = nullptr;
std::once_flag ModelManager::initInstanceFlag;

ModelManager* ModelManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = new ModelManager();
		});
	return instance;
}

void ModelManager::Initialize(DirectXManager* dxManager, SrvManager* srvManager)
{
	modelLoader = std::make_unique<ModelLoader>();
	modelLoader->Initialize(dxManager, srvManager);
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& fileName)
{
	// 読み込み済みモデルを検索
	if (models.contains(fileName)) {
		// 読み込み済みなら早期return
		return;
	}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelLoader.get(), directoryPath, fileName);

	// モデルの生成とファイルの読み込み
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないメッシュは今回は非対応
		assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回は非対応

		// Faceを解析する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみをサポート

			// Vertexを解析する
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				Model::VertexData vertex;
				vertex.position = { position.x, position.y, position.z, 1.0f };
				vertex.normal = { normal.x, normal.y, normal.z };
				vertex.texcoord = { texcoord.x, texcoord.y };
				// aiProcess_MakeLeftHandedは z*=-1 で、右手->左手に変換されるらしいので手動で対処
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				model->SetVertices(vertex);
			}
		}
	}

	// materialの解析を行う
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			model->SetTexturePath(directoryPath + "/" + textureFilePath.C_Str());
		}
	}
	model->CreateVertexResource();
	// モデルをmapコンテナに格納する
	models.insert(std::make_pair(fileName, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// 読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}

	// ファイル名が一致しなければnull
	return nullptr;
}
