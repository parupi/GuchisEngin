#pragma once  
#include "3d/Object/Object3d.h" // Object3d クラスの宣言をインクルード  
#include "3d/Object/Renderer/ModelRenderer.h"
#include "3d/Object/Model/SkinnedModel.h"
#include <base/Particle/ParticleEmitter.h>

enum class JumpState {
	None,
	JumpUp,
	JumpIdle,
	JumpLand
};

class Player : public Object3d // "public" を追加して正しい継承を指定  
{  
public: // メンバ関数
	Player(std::string objectName);
	~Player() override = default;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update() override;
	void Draw() override;

private:
	Vector3 velocity_{};

	JumpState jumpState_ = JumpState::None;
	std::unique_ptr<ParticleEmitter> particleEmitter_ = nullptr;

	Object3d* weapon_ = nullptr;
};
