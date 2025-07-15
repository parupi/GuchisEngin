#pragma once
#include <3d/WorldTransform.h>
#include <memory>
class BaseModel;

class BaseRenderer
{
public:

	virtual void Update(WorldTransform* parentTransform) = 0;

	virtual void Draw(WorldTransform* parentTransform) = 0;
#ifdef _DEBUG
	virtual void DebugGui(size_t index) = 0;
#endif // DEBUG

	std::string name_;
	std::unique_ptr<WorldTransform> localTransform_;

	virtual WorldTransform* GetWorldTransform() const = 0;
	virtual BaseModel* GetModel() const = 0;
};

