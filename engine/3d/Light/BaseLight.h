#pragma once
#include <Vector3.h>

class BaseLight {
public:
    virtual ~BaseLight() = default;
    virtual void Update() = 0;
    //virtual LightType GetType() const = 0;

    Vector3 position_;
    Vector3 direction_;
    Vector3 color_;
    float intensity_;
};