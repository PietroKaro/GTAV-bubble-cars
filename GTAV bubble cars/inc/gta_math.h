#ifndef GTA_MATH
#define GTA_MATH

#include <Windows.h>
#include <cmath>
#include <string>

// ---------- Original Vector3 struct from AB SHV but expanded ----------

#pragma pack(push, 1)
struct Vector3
{
	float x;
	DWORD struct_paddingx;
	float y;
	DWORD struct_paddingy;
	float z;
	DWORD struct_paddingz;

	constexpr Vector3(float cx, float cy, float cz) : x{ cx }, y{ cy }, z{ cz },
		struct_paddingx{}, struct_paddingy{}, struct_paddingz{} {}

	constexpr Vector3() : Vector3{ 0.0f, 0.0f, 0.0f } {}

	float sqrMag() const { return (x * x) + (y * y) + (z * z); }
	float mag() const { return sqrt(sqrMag()); }
	Vector3 normalized() const;

	operator std::string() const { return "X: " + std::to_string(x) + "  Y: " + std::to_string(y) + "  Z: " + std::to_string(z); }
	Vector3& operator +=(const Vector3& right);
	Vector3& operator -=(const Vector3& right);
	Vector3& operator *=(float k);
	Vector3& operator /=(float k);
};
#pragma pack(pop)

namespace def_vector3
{
	constexpr Vector3 ZERO;
	constexpr Vector3 ONE{ 1.0f, 1.0f, 1.0f };
	constexpr Vector3 UNIT_X{ 1.0f, 0.0f, 0.0f };
	constexpr Vector3 UNIT_Y{ 0.0f, 1.0f, 0.0f };
	constexpr Vector3 UNIT_Z{ 0.0f, 0.0f, 1.0f };
	constexpr Vector3 WORLD_NORTH{ 0.0f, 1.0f, 0.0f };
	constexpr Vector3 WORLD_SOUTH{ 0.0f, -1.0f, 0.0f };
	constexpr Vector3 WORLD_EAST{ 1.0f, 0.0f, 0.0f };
	constexpr Vector3 WORLD_WEST{ -1.0f, 0.0f, 0.0f };
	constexpr Vector3 WORLD_TOP{ 0.0f, 0.0f, 1.0f };
	constexpr Vector3 WORLD_DOWN{ 0.0f, 0.0f, -1.0f };
}

bool operator ==(const Vector3& left, const Vector3& right);
bool operator !=(const Vector3& left, const Vector3& right);
Vector3 operator -(const Vector3& vec);
Vector3 operator +(Vector3 left, Vector3 right);
Vector3 operator -(Vector3 left, Vector3 right);
Vector3 operator *(Vector3 left, float k);
Vector3 operator /(Vector3 left, float k);

inline float dotProduct(const Vector3& left, const Vector3& right)
{
	return (left.x * right.x) + (left.y * right.y) + (left.z * right.z);
}

Vector3 crossProduct(const Vector3& left, const Vector3& right);
void normalize(Vector3& v);

#endif // !GTA_MATH