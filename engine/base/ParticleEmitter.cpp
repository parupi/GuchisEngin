#include "ParticleEmitter.h"
#include "ParticleManager.h"

void ParticleEmitter::Initialize(ParticleManager* manager, std::string name)
{
	particleManager_ = manager;
	emitter.name = name;
	emitter.frequency = 0.5f;
	emitter.isActive = true;
}

void ParticleEmitter::Update(Vector3 Position, uint32_t count)
{
	if (emitter.isActive) {
		emitter.transform.translate = Position;
		emitter.count = count;

		emitter.frequencyTime += kDeltaTime;
		if (emitter.frequency <= emitter.frequencyTime) {
			// パーティクルを生成してグループに追加
			Emit();
			emitter.frequencyTime -= emitter.frequency;
		}
	}
}

void ParticleEmitter::Emit()
{
	particleManager_->Emit(emitter.name, emitter.transform.translate, emitter.count);
}
