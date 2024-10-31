#pragma once

struct Vector3;
struct Matrix4x4;

struct Quaternion final {
    float x;
    float y;
    float z;
    float w;

    // コンストラクタ
    Quaternion(float x = 0, float y = 0, float z = 0, float w = 1);

    // クォータニオンの乗算
    Quaternion operator*(const Quaternion& q) const;

    // 等価・非等価演算子
    bool operator==(const Quaternion& q) const;
    bool operator!=(const Quaternion& q) const;

    // 加算・減算演算子
    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator-(const Quaternion& q) const;
    Quaternion& operator+=(const Quaternion& q);
    Quaternion& operator-=(const Quaternion& q);

    // スカラー倍演算子
    Quaternion operator*(float scalar) const;
    Quaternion& operator*=(float scalar);
};

// 単位クォータニオン（乗法の単位元）
Quaternion Identity();
// クォータニオンの共役
Quaternion Conjugate(const Quaternion& q);
// クォータニオンのノルム
float Norm(const Quaternion& q);
// クォータニオンの正規化
Quaternion Normalize(const Quaternion& q);
// クォータニオンの逆
Quaternion Inverse(const Quaternion& q);
// クォータニオンの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
// 球面線形補間
Quaternion Slerp(Quaternion q0, Quaternion q1, float t);
// 内積
float Dot(const Quaternion& q0, const Quaternion& q1);

// ImGuiを使ったクォータニオンの描画
void PrintOnImGui(const Quaternion& q, const char* label = "Quaternion");
