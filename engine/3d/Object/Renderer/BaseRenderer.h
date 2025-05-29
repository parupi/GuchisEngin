#pragma once
#include <WorldTransform.h>
class BaseRenderer
{
public:
	virtual void Update(WorldTransform* parentTransform) = 0;

	virtual void Draw(WorldTransform* parentTransform) = 0;
#ifdef _DEBUG
	virtual void DebugGui(size_t index) = 0;
#endif // DEBUG
	
	std::string name;
};

