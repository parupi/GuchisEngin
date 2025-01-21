#pragma once
#include <cstdint>
// コリジョン種別IDを定義
enum class CollisionTypeIdDef : uint32_t
{
	kDefault = 0,	// デフォルト
	kPlayerWeapon,	// プレイヤーの武器
	kEnemy,			// 敵
	kNone			// 当たり判定なし
};