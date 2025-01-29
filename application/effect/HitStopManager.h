#pragma once
#include <mutex>
class HitStopManager
{
public:
	// 更新
	void Update();
	bool IsHitStopActive() const { return isActive; }
	void Trigger(int duration);

private:
	bool isActive = false;
	int timer = 0;

};

