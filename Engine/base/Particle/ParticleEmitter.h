#pragma once
#include <math/function.h>
#include <base/Particle/ParticleManager.h>
#include <base/utility/DeltaTime.h>
class ParticleEmitter
{
public:
	// 初期化
	void Initialize(std::string name_);
	// 更新
	void Update(Vector3 Position, uint32_t count);
	// 発生
	void Emit() const;
private:

	struct Emitter {
		std::string name_; //!< パーティクルの名前
		EulerTransform transform; //!< エミッタのTransform
		uint32_t count; //!< 発生数
		float frequency; //!< 発生頻度
		float frequencyTime; //!< 頻度用時刻
		bool isActive;
	};

private:
	//ParticleManager* particleManager_;
	bool emitAll_ = false;
	Emitter emitter{};
	float kDeltaTime = DeltaTime::GetDeltaTime();

public:
	void SetFrequency(float time) { emitter.frequency = time; }
	void SetActiveFlag(bool flag) { emitter.isActive = flag; }

};