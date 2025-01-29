#pragma once
#include <memory>
#include <Sprite.h>
#include <Camera.h>
class HitEffect
{
public:
	// 初期化
	void Initialize(Camera* camera);
	// 終了
	void Finalize();
	// 更新
	void Update(const Vector3& position);
	// 描画
	void Draw();

public: // アクセッサ
	bool GetIsAlive() const { return isAllive; }


private:


	// ダメージエフェクト用のスプライト
	std::unique_ptr<Sprite> sprite_;

	Camera* camera_;

	bool isAllive = true;

};

