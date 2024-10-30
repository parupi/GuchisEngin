#include "Quaternion.h"
#include <cmath>
#include <imgui.h>

// Constructor
Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

// Quaternion multiplication
Quaternion Quaternion::operator*(const Quaternion& q) const {
    return Quaternion(
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    );
}

// Identity quaternion (multiplicative identity)
Quaternion Quaternion::Identity() {
    return Quaternion(0, 0, 0, 1);
}

// Conjugate of quaternion
Quaternion Quaternion::Conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

// Norm (magnitude) of quaternion
float Quaternion::Norm() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

// Normalize quaternion to unit length
Quaternion Quaternion::Unit() const {
    float norm = Norm();
    if (norm == 0) return Quaternion(0, 0, 0, 1); // Return identity if norm is zero
    return Quaternion(x / norm, y / norm, z / norm, w / norm);
}

// Inverse quaternion
Quaternion Quaternion::Inverse() const {
    float normSq = x * x + y * y + z * z + w * w;
    if (normSq == 0) return Quaternion(0, 0, 0, 1); // Return identity if norm is zero
    Quaternion conj = Conjugate();
    return Quaternion(conj.x / normSq, conj.y / normSq, conj.z / normSq, conj.w / normSq);
}

// Display quaternion values in ImGui
void Quaternion::DisplayInImGui(const char* label) const {
    ImGui::Begin("Quaternion");
    ImGui::Text("%s: (x: %.3f, y: %.3f, z: %.3f, w: %.3f)", label, x, y, z, w);
    ImGui::End();
}