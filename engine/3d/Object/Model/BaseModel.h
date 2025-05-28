#pragma once

class Object3d;
class ModelRender;
class BaseModel
{
public:
	//virtual ~BaseModel() = default;
	virtual void Update() = 0;
	virtual void Draw() = 0;
#ifdef _DEBUG
	virtual void DebugGui(ModelRender* render) = 0;
#endif // _DEBUG
};

