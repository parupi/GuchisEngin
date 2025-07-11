#pragma once
#include <math/Vector3.h>

struct AABBData {
	Vector3 offsetMax = { 0.5f, 0.5f, 0.5f };
	Vector3 offsetMin = { -0.5f, -0.5f, -0.5f };
	bool isActive = true;
};

struct SphereData {
	float radius = 0.5f;
	Vector3 offset = { 0.0f, 0.0f, 0.0f };
	bool isActive = true;
};