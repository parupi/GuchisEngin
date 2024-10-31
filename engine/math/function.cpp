#include "function.h"

float Normalize(float value) {
	// 値が0でない場合、絶対値が1になるように正規化
	if (value != 0.0f) {
		return value / fabs(value); // 符号を保ちながら1または-1に正規化
	}
	return 0.0f; // 0の時はそのまま0を返す
}

float Lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) { return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, translate.x, translate.y, translate.z, 1}; }

Matrix4x4 MakeScaleMatrix(const Vector3& scale) { return {scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1}; }

//Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
//	Vector3 result;
//	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
//	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
//	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
//	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
//	assert(w != 0.0f);
//	result.x /= w;
//	result.y /= w;
//	result.z /= w;
//	return result;
//}

Vector3 Transformm_(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

//Matrix4x4 Inverse(const Matrix4x4& m) {
//	float A =
//	    m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
//	    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
//	    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
//	    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
//	    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
//	    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
//
//	return {
//	    (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
//	     m.m[1][1] * m.m[2][3] * m.m[3][2]) /
//	        A,
//
//	    (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
//	     m.m[0][1] * m.m[2][3] * m.m[3][2]) /
//	        A,
//
//	    (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
//	     m.m[0][1] * m.m[1][3] * m.m[3][2]) /
//	        A,
//
//	    (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
//	     m.m[0][1] * m.m[1][3] * m.m[2][2]) /
//	        A,
//
//	    (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
//	     m.m[1][0] * m.m[2][3] * m.m[3][2]) /
//	        A,
//
//	    (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
//	     m.m[0][0] * m.m[2][3] * m.m[3][2]) /
//	        A,
//
//	    (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
//	     m.m[0][0] * m.m[1][3] * m.m[3][2]) /
//	        A,
//
//	    (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][3] -
//	     m.m[0][0] * m.m[1][3] * m.m[2][2]) /
//	        A,
//
//	    (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
//	     m.m[1][0] * m.m[2][3] * m.m[3][1]) /
//	        A,
//
//	    (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
//	     m.m[0][0] * m.m[2][3] * m.m[3][1]) /
//	        A,
//
//	    (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
//	     m.m[0][0] * m.m[1][3] * m.m[3][1]) /
//	        A,
//
//	    (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
//	     m.m[0][0] * m.m[1][3] * m.m[2][1]) /
//	        A,
//
//	    (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
//	     m.m[1][0] * m.m[2][2] * m.m[3][1]) /
//	        A,
//
//	    (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
//	     m.m[0][0] * m.m[2][2] * m.m[3][1]) /
//	        A,
//
//	    (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
//	     m.m[0][0] * m.m[1][2] * m.m[3][1]) /
//	        A,
//
//	    (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
//	     m.m[0][0] * m.m[1][2] * m.m[2][1]) /
//	        A
//
//	};
//}
//
//Matrix4x4 Transpose(const Matrix4x4& m) {
//	return {m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0], m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1], m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2], m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]};
//}
//
//Matrix4x4 MakeIdentity4x4() { return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; }
//
//Matrix4x4 MakeRotateXMatrix(float radian) { return {1, 0, 0, 0, 0, std::cosf(radian), std::sinf(radian), 0, 0, std::sinf(-radian), std::cosf(radian), 0, 0, 0, 0, 1}; };
//
//Matrix4x4 MakeRotateYMatrix(float radian) { return {std::cosf(radian), 0, std::sinf(-radian), 0, 0, 1, 0, 0, std::sinf(radian), 0, std::cosf(radian), 0, 0, 0, 0, 1}; };
//
//Matrix4x4 MakeRotateZMatrix(float radian) { return {std::cosf(radian), std::sinf(radian), 0, 0, std::sinf(-radian), std::cosf(radian), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; };

Matrix4x4 MakeRotateXYZMatrix(Vector3 rotate) {
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = (rotateYMatrix * rotateXMatrix * rotateZMatrix);
	return rotateMatrix;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;

	return {scaleMatrix * rotateMatrix * translateMatrix};
}

float cotf(float theta) { return 1.0f / std::tanf(theta); }

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	return {1.0f / aspectRatio * cotf(fovY / 2.0f), 0, 0, 0, 0, cotf(fovY / 2.0f), 0, 0, 0, 0, farClip / (farClip - nearClip), 1.0f, 0, 0, -nearClip * farClip / (farClip - nearClip), 0};
};

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	return {2.0f / (right - left),           0,   0, 0, 0, 2.0f / (top - bottom), 0, 0, 0, 0, 1.0f / (farClip - nearClip), 0, (left + right) / (left - right), (top + bottom) / (bottom - top),
	        nearClip / (nearClip - farClip), 1.0f};
};

Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	return {width / 2.0f, 0, 0, 0, 0, -height / 2.0f, 0, 0, 0, 0, maxDepth - minDepth, 0, left + width / 2.0f, top + height / 2.0f, minDepth, 1.0f};
}
Vector3 Cross(const Vector3& v1, const Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; };

Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
	Vector3 zaxis = Normalize(target - eye);     // forward
	Vector3 xaxis = Normalize(Cross(up, zaxis)); // right
	Vector3 yaxis = Cross(zaxis, xaxis);         // up

	Matrix4x4 result = {
	    xaxis.x,
	    yaxis.x,
	    zaxis.x,
	    0,
	    xaxis.y,
	    yaxis.y,
	    zaxis.y,
	    0,
	    xaxis.z,
	    yaxis.z,
	    zaxis.z,
	    0,
	    -(xaxis.x * eye.x + xaxis.y * eye.y + xaxis.z * eye.z),
	    -(yaxis.x * eye.x + yaxis.y * eye.y + yaxis.z * eye.z),
	    -(zaxis.x * eye.x + zaxis.y * eye.y + zaxis.z * eye.z),
	    1};

	return result;
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};
	return result;
}

Vector3 ExtractTranslation(const Matrix4x4& matrix) {
	Vector3 translation;
	translation.x = matrix.m[3][0];
	translation.y = matrix.m[3][1];
	translation.z = matrix.m[3][2];
	return translation;
}

//Matrix4x4 ScaleMatrixFromVector3(const Vector3& scale)
//{
//	Matrix4x4 scaleMatrix = {};
//	scaleMatrix.m[0][0] = scale.x;  // X方向のスケール
//	scaleMatrix.m[1][1] = scale.y;  // Y方向のスケール
//	scaleMatrix.m[2][2] = scale.z;  // Z方向のスケール
//	scaleMatrix.m[3][3] = 1.0f;     // W成分は1.0
//
//	return scaleMatrix;
//}
//
//Matrix4x4 TranslationMatrixFromVector3(const Vector3& translate)
//{
//	Matrix4x4 translationMatrix = {};
//	translationMatrix.m[0][0] = 1.0f;  // 単位行列の成分
//	translationMatrix.m[1][1] = 1.0f;  // 単位行列の成分
//	translationMatrix.m[2][2] = 1.0f;  // 単位行列の成分
//	translationMatrix.m[3][3] = 1.0f;  // 単位行列の成分
//
//	translationMatrix.m[3][0] = translate.x;  // X方向の移動
//	translationMatrix.m[3][1] = translate.y;  // Y方向の移動
//	translationMatrix.m[3][2] = translate.z;  // Z方向の移動
//
//	return translationMatrix;
//}

//Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
//{
//	// 回転軸ベクトルを正規化
//	Vector3 normalizedAxis = Normalize(axis);
//	float x = normalizedAxis.x;
//	float y = normalizedAxis.y;
//	float z = normalizedAxis.z;
//
//	// 三角関数を事前に計算
//	float cosTheta = std::cos(angle);
//	float sinTheta = std::sin(angle);
//	float oneMinusCosTheta = 1.0f - cosTheta;
//
//	// 回転行列を生成
//	Matrix4x4 rotationMatrix;
//	rotationMatrix.m[0][0] = cosTheta + x * x * oneMinusCosTheta;
//	rotationMatrix.m[0][1] = x * y * oneMinusCosTheta + z * sinTheta;
//	rotationMatrix.m[0][2] = x * z * oneMinusCosTheta - y * sinTheta;
//	rotationMatrix.m[0][3] = 0.0f;
//
//	rotationMatrix.m[1][0] = x * y * oneMinusCosTheta - z * sinTheta;
//	rotationMatrix.m[1][1] = cosTheta + y * y * oneMinusCosTheta;
//	rotationMatrix.m[1][2] = y * z * oneMinusCosTheta + x * sinTheta;
//	rotationMatrix.m[1][3] = 0.0f;
//
//	rotationMatrix.m[2][0] = z * x * oneMinusCosTheta + y * sinTheta;
//	rotationMatrix.m[2][1] = z * y * oneMinusCosTheta - x * sinTheta;
//	rotationMatrix.m[2][2] = cosTheta + z * z * oneMinusCosTheta;
//	rotationMatrix.m[2][3] = 0.0f;
//
//	rotationMatrix.m[3][0] = 0.0f;
//	rotationMatrix.m[3][1] = 0.0f;
//	rotationMatrix.m[3][2] = 0.0f;
//	rotationMatrix.m[3][3] = 1.0f;
//
//	return rotationMatrix;
//}



Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to)
{
	Vector3 fromNorm = Normalize(from);
	Vector3 toNorm = Normalize(to);

	// 回転軸を計算
	Vector3 axis = Cross(fromNorm, toNorm);
	float sinAngle = std::sqrt(Dot(axis, axis));  // 軸の長さは sin(角度) と等しい
	float cosAngle = Dot(fromNorm, toNorm);       // 内積は cos(角度) と等しい

	// 特殊ケース: ベクトルがほぼ一致する場合（回転不要）
	if (sinAngle < 1e-6f) {
		return MakeIdentity4x4();
	}

	// 特殊ケース: 180度の回転（ベクトルが正反対の場合）
	if (cosAngle < -1.0f + 1e-6f) {
		// 例えば (1,0,0) を (0,1,0) にマッピングするような任意の軸が必要
		axis = std::abs(fromNorm.x) > 0.9f ? Vector3{ 0, 1, 0 } : Vector3{ 1, 0, 0 };
		axis = Normalize(Cross(fromNorm, axis));
		sinAngle = 1.0f;
		cosAngle = -1.0f;
	}
	else {
		// 軸を正規化
		axis = Normalize(axis);
	}

	// ロドリゲスの回転公式を使って回転行列を作成
	float x = axis.x, y = axis.y, z = axis.z;
	float oneMinusCos = 1.0f - cosAngle;

	Matrix4x4 rotationMatrix;
	rotationMatrix.m[0][0] = cosAngle + x * x * oneMinusCos;
	rotationMatrix.m[0][1] = x * y * oneMinusCos + z * sinAngle;
	rotationMatrix.m[0][2] = x * z * oneMinusCos - y * sinAngle;
	rotationMatrix.m[0][3] = 0.0f;

	rotationMatrix.m[1][0] = y * x * oneMinusCos - z * sinAngle;
	rotationMatrix.m[1][1] = cosAngle + y * y * oneMinusCos;
	rotationMatrix.m[1][2] = y * z * oneMinusCos + x * sinAngle;
	rotationMatrix.m[1][3] = 0.0f;

	rotationMatrix.m[2][0] = z * x * oneMinusCos + y * sinAngle;
	rotationMatrix.m[2][1] = z * y * oneMinusCos - x * sinAngle;
	rotationMatrix.m[2][2] = cosAngle + z * z * oneMinusCos;
	rotationMatrix.m[2][3] = 0.0f;

	rotationMatrix.m[3][0] = 0.0f;
	rotationMatrix.m[3][1] = 0.0f;
	rotationMatrix.m[3][2] = 0.0f;
	rotationMatrix.m[3][3] = 1.0f;

	return rotationMatrix;
}