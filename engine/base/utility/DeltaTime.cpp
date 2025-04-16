#include "DeltaTime.h"
#include <thread>

std::chrono::high_resolution_clock::time_point DeltaTime::reference_;
float DeltaTime::deltaTime_ = 0.0f;

void DeltaTime::Initialize()
{
	reference_ = std::chrono::high_resolution_clock::now();
	deltaTime_ = 0.0f;
}

void DeltaTime::Update()
{
    // 1/60秒ぴったりの時間
    constexpr uint64_t microSecPer60FPS = static_cast<uint64_t>(1000000.0f / 60.0f);
    constexpr uint64_t microSecCheck = static_cast<uint64_t>(1000000.0f / 65.0f);
    const std::chrono::microseconds kMinTime(microSecPer60FPS);
    const std::chrono::microseconds kMinCheckTime(microSecCheck);

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    // 十分な時間が経っていなければスリープ
    if (elapsed < kMinTime || elapsed < kMinCheckTime) {
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

    // Δタイムを秒単位で計算
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> delta = currentTime - reference_;
    deltaTime_ = delta.count();

    // 基準時間更新
    reference_ = currentTime;
}
