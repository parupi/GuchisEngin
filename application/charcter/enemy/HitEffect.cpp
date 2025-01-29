#include "HitEffect.h"
#include "random"

void HitEffect::Initialize(Camera* camera)
{
	camera_ = camera;

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("resource/damageSprite.png");
	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->SetSize({ 4.0f, 56.0f });
	
	// 乱数の設定
	std::random_device rd;
	std::mt19937 gen(rd());  // メルセンヌ・ツイスター乱数生成器
	std::uniform_real_distribution<float> dist(0.0f, 3.14f);  // 0 〜 3.14 の範囲

	// ランダムな回転角度を取得
	float randomRotation = dist(gen);

	// スプライトの回転をランダムに設定
	sprite_->SetRotation(randomRotation);
}

void HitEffect::Finalize()
{
}

void HitEffect::Update(const Vector3& position)
{

	// 敵のスクリーン座標を計算
	int screenWidth = 1280;
	int screenHeight = 720;
	// スクリーン座標の位置にダメージエフェクトを置く
	Vector2 screenPosition = camera_->WorldToScreen(position, screenWidth, screenHeight);
	sprite_->SetPosition(screenPosition);

	// 現在のサイズを取得
	Vector2 spriteSize = sprite_->GetSize();

	// 現在のUITransformを取得
	Vector2 uvTransform = sprite_->GetUVPosition();
	// ダメージエフェクトのスプライトは透明度を下げ続ける(透明じゃない = 起動中みたいな)
	float spriteAlpha = sprite_->GetColor().w;
	if (spriteAlpha > 0.0f) {
		// アルファ値を下げ続ける
		spriteAlpha -= 0.08f;
		// uv位置を変更する
		uvTransform += {0.0f, 0.1f};
		// サイズを変える(yを伸ばす)
		spriteSize += {0.0f, 12.0f};
	}
	else {
		isAllive = false;
	}
	// 色(透明度)を更新
	sprite_->SetColor({ 1.0f, 1.0f, 1.0f, spriteAlpha });
	// uv位置を設定
	sprite_->SetUVPosition(uvTransform);
	sprite_->SetSize(spriteSize);
	sprite_->Update();
}

void HitEffect::Draw()
{
	sprite_->Draw();
}
