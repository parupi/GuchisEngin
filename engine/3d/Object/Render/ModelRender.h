#pragma once
#include "BaseRender.h"
#include <Model/Model.h>
#include <CameraManager.h>
#include <Camera/Camera.h>
class ModelRender : public BaseRender
{
public:
	ModelRender(const std::string& renderName, const std::string& filePath);
	~ModelRender() = default;

	void Update(WorldTransform* parentTransform) override;

	void Draw(WorldTransform* parentTransform) override;

	void SetModel(const std::string& filePath);

#ifdef _DEBUG
	void DebugGui(size_t index) override;
#endif // DEBUG
private:
	BaseModel* model_;
	std::unique_ptr<WorldTransform> localTransform_;

	Camera* camera_ = CameraManager::GetInstance()->GetActiveCamera().get();
};

