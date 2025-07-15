#pragma once
#include <base/DirectXManager.h>
#include <base/PSOManager.h>
class BaseOffScreen
{
public:
	virtual ~BaseOffScreen() = default;

	//virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	bool IsActive() const { return isActive_; };
protected:
	DirectXManager* dxManager_ = nullptr;
	PSOManager* psoManager_ = nullptr;

	bool isActive_;
};

