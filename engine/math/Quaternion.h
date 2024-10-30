#pragma once

struct Quaternion
{
    float x;
    float y;
    float z;
    float w;

    // Constructor
    Quaternion(float x = 0, float y = 0, float z = 0, float w = 1);

    // Quaternion multiplication
    Quaternion operator*(const Quaternion& q) const;

    // Identity quaternion (multiplicative identity)
    static Quaternion Identity();

    // Conjugate of quaternion
    Quaternion Conjugate() const;

    // ノルム
    float Norm() const;

    // 正規化
    Quaternion Unit() const;

    // 逆
    Quaternion Inverse() const;

    // ImGuiを使った描画
    void DisplayInImGui(const char* label) const;
};