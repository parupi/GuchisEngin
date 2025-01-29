#include "HitStopManager.h"

void HitStopManager::Update()
{
    if (isActive) {
        if (timer > 0) {
            timer--;
        }
        else {
            isActive = false;
        }
    }
}

void HitStopManager::Trigger()
{
    isActive = true;
    timer = startTime;
}
