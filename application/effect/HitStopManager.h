#pragma once
#include <mutex>
class HitStopManager
{
public:
	// 更新
	void Update();
	bool IsHitStopActive() const { return isActive; }
	void Trigger();
	void SetTimer(int duration) { startTime = duration; }

private:
	bool isActive = false;
	int timer = 0;
	int startTime = 3;
};

