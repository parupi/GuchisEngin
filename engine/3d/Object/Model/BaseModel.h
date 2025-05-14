#pragma once
class BaseModel
{
public:
	//virtual ~BaseModel() = default;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

